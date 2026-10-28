/* Native signal capture: persist a minimal record with async-signal-safe write,
 * then replay it through xlog on the next open, outside signal context. */
#include "XLogCrashHandler.h"
#include "mars/xlog/xloggerbase.h"
#include "mars/xlog/appender.h"
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include <string>

namespace rnoh {
namespace xlog {
namespace {
constexpr int signals[] = {SIGABRT, SIGILL, SIGSEGV, SIGFPE, SIGBUS, SIGPIPE};
struct sigaction oldActions[6];
volatile sig_atomic_t installed = 0;
volatile sig_atomic_t crashFd = -1;
std::string pendingPath;

void SignalHandler(int sig) {
    // A fixed-size binary record avoids formatting, allocation and xlog locks.
    if (crashFd >= 0) {
        int record = sig;
        (void)write(crashFd, &record, sizeof(record));
    }
    // Restore the previous handlers before redelivery, including the platform's
    // crash reporter. A pending signal is delivered when this handler returns.
    for (unsigned i = 0; i < sizeof(signals) / sizeof(signals[0]); ++i) {
        (void)sigaction(signals[i], &oldActions[i], nullptr);
    }
    installed = 0;
    (void)raise(sig);
}
}

void PrepareCrashLog(const std::string &logPath) {
    pendingPath = logPath + "/xlog_crash.pending";
    // The descriptor remains open for the lifetime of the process so a signal
    // cannot race with close() and accidentally write to a reused descriptor.
    if (crashFd < 0) {
        crashFd = open(pendingPath.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_CLOEXEC, 0600);
    }
    int fd = open(pendingPath.c_str(), O_RDONLY | O_CLOEXEC);
    if (fd < 0) return;
    int sig;
    bool found = false;
    while (read(fd, &sig, sizeof(sig)) == sizeof(sig)) {
        char message[128];
        snprintf(message, sizeof(message), "Signal %d was raised (recovered after restart).", sig);
        XLoggerInfo info{};
        info.level = kLevelFatal;
        info.tag = "crash";
        info.filename = "";
        info.func_name = "";
        gettimeofday(&info.timeval, nullptr);
        info.pid = getpid();
        info.tid = gettid();
        info.maintid = info.tid;
        xlogger_Write(&info, message);
        found = true;
    }
    close(fd);
    if (found) {
        mars::xlog::appender_flush_sync();
        // A repeat after interruption is preferable to losing an unprocessed record.
        int truncateFd = open(pendingPath.c_str(), O_WRONLY | O_TRUNC | O_CLOEXEC);
        if (truncateFd >= 0) close(truncateFd);
    }
}

bool InstallUncaughtSignalHandler() {
    if (installed) return true;
    if (crashFd < 0) return false;
    struct sigaction sa{};
    sa.sa_handler = SignalHandler;
    sigemptyset(&sa.sa_mask);
    // Block all handled signals while recording to prevent recursive handlers.
    for (int sig : signals) sigaddset(&sa.sa_mask, sig);
    unsigned i = 0;
    for (; i < sizeof(signals) / sizeof(signals[0]); ++i) {
        if (sigaction(signals[i], &sa, &oldActions[i]) != 0) {
            while (i > 0) { --i; (void)sigaction(signals[i], &oldActions[i], nullptr); }
            return false;
        }
    }
    installed = 1;
    return true;
}

void UninstallUncaughtSignalHandler() {
    if (!installed) return;
    for (unsigned i = 0; i < sizeof(signals) / sizeof(signals[0]); ++i) {
        struct sigaction current{};
        if (sigaction(signals[i], nullptr, &current) == 0 &&
            !(current.sa_flags & SA_SIGINFO) && current.sa_handler == SignalHandler) {
            (void)sigaction(signals[i], &oldActions[i], nullptr);
        }
    }
    installed = 0;
}
}
}

