/*
 * 对标 iOS XLogCrashHandler.mm
 * 鸿蒙平台崩溃信号处理器
 *
 * 功能：
 * 1. 捕获 SIGABRT/SIGILL/SIGSEGV/SIGFPE/SIGBUS/SIGPIPE 信号
 * 2. 获取调用栈回溯（最多128帧）
 * 3. 将崩溃信息以 kLevelFatal 写入 xlog
 * 4. 使用原子计数器防递归（最多10次）
 */

#include "XLogCrashHandler.h"

#include <signal.h>
#include <execinfo.h>
#include <atomic>
#include <unistd.h>
#include <string.h>

#include "mars/xlog/xloggerbase.h"
#include "mars/xlog/xlogger.h"

namespace rnoh {
namespace xlog {

static std::atomic<int32_t> sUncaughtExceptionCount(0);
static const int32_t UNCAUGHT_EXCEPTION_MAXIMUM = 10;

// 保存原始信号处理函数，用于卸载时恢复
static struct sigaction sOldActions[6];
static bool sHandlerInstalled = false;

// 获取调用栈（对标 iOS +[XLogCrashHandler backtrace]）
static void logBacktrace() {
    void* callstack[128];
    int frames = backtrace(callstack, 128);
    char** strs = backtrace_symbols(callstack, frames);

    if (strs != nullptr) {
        for (int i = 0; i < frames; i++) {
            XLoggerInfo info;
            info.level = kLevelFatal;
            info.tag = "crash";
            info.filename = "";
            info.func_name = "";
            info.line = 0;
            gettimeofday(&info.timeval, NULL);
            info.pid = getpid();
            info.tid = gettid();
            info.maintid = gettid();
            xlogger_Write(&info, strs[i]);
        }
        free(strs);
    }
}

// 信号处理函数（对标 iOS SignalHandler）
static void SignalHandler(int sig) {
    int32_t exceptionCount = sUncaughtExceptionCount.fetch_add(1, std::memory_order_relaxed) + 1;
    if (exceptionCount > UNCAUGHT_EXCEPTION_MAXIMUM) {
        return;
    }

    // 记录信号信息
    char reason[128];
    snprintf(reason, sizeof(reason), "Signal %d was raised.", sig);

    XLoggerInfo info;
    info.level = kLevelFatal;
    info.tag = "crash";
    info.filename = "";
    info.func_name = "";
    info.line = 0;
    gettimeofday(&info.timeval, NULL);
    info.pid = getpid();
    info.tid = gettid();
    info.maintid = gettid();
    xlogger_Write(&info, reason);

    // 记录调用栈
    logBacktrace();

    // 捕获后卸载信号处理器（与 iOS 行为一致）
    UninstallUncaughtSignalHandler();
}

// 安装信号处理器（对标 iOS InstallUncaughtSignalHandler）
void InstallUncaughtSignalHandler(void) {
    if (sHandlerInstalled) {
        return;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SignalHandler;
    sigemptyset(&sa.sa_mask);

    // 捕获与 iOS 相同的 6 种信号
    sigaction(SIGABRT, &sa, &sOldActions[0]);
    sigaction(SIGILL,  &sa, &sOldActions[1]);
    sigaction(SIGSEGV, &sa, &sOldActions[2]);
    sigaction(SIGFPE,  &sa, &sOldActions[3]);
    sigaction(SIGBUS,  &sa, &sOldActions[4]);
    sigaction(SIGPIPE, &sa, &sOldActions[5]);

    sHandlerInstalled = true;
}

// 卸载信号处理器（对标 iOS UninstallUncaughtSignalHandler）
void UninstallUncaughtSignalHandler(void) {
    if (!sHandlerInstalled) {
        return;
    }

    // 恢复默认信号处理
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGILL,  &sa, nullptr);
    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGFPE,  &sa, nullptr);
    sigaction(SIGBUS,  &sa, nullptr);
    sigaction(SIGPIPE, &sa, nullptr);

    sHandlerInstalled = false;
}

} // namespace xlog
} // namespace rnoh
