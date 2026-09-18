/*
 * 对标 iOS XLogBridge.mm + LogHelper.mm
 * 鸿蒙平台 Xlog TurboModule 核心实现
 *
 * 功能：
 * 1. open  — 初始化 xlog，打开日志文件，重定向 RN 日志
 * 2. close — 关闭 xlog
 * 3. log   — 写入一条日志（构建 XLoggerInfo + xlogger_Write）
 * 4. installUncaughtCrashHandler   — 安装崩溃信号处理器
 * 5. uninstallUncaughtCrashHandler — 卸载崩溃信号处理器
 */

#include "RNXLogModule.h"
#include "XLogCrashHandler.h"

#include "mars/xlog/xloggerbase.h"
#include "mars/xlog/appender.h"
#include "mars/xlog/xlogger.h"

#include "XLogNativeLogger.h"
#include <jsi/jsi.h>

#include <sys/time.h>
#include <unistd.h>

namespace rnoh {
using namespace facebook;
namespace {
struct XLogOpenConfig {
    std::string logPath = "/data/storage/el2/base/haps/entry/cache";
    int level = kLevelAll;
    bool showConsoleLog = true;
    int appenderMode = 0;
    std::string namePrefix = "Test";
};

XLogOpenConfig g_config;
}

void RNXLogModule::Register(const std::string &logPath,
                            int level,
                            bool showConsoleLog,
                            int appenderMode,
                            const std::string &namePrefix)
{
    g_config.logPath = logPath;
    g_config.level = level;
    g_config.showConsoleLog = showConsoleLog;
    g_config.appenderMode = appenderMode;
    g_config.namePrefix = namePrefix;
}

static void WriteLog(TLogLevel level, const char* tag, const char* message)
{
    XLoggerInfo info;
    info.level = level;
    info.tag = tag;
    info.filename = "";
    info.func_name = "";
    info.line = 0;
    gettimeofday(&info.timeval, NULL);
    info.pid = getpid();
    info.tid = gettid();
    info.maintid = gettid();
    xlogger_Write(&info, message);
}

static TLogLevel MapLogLevel(unsigned int logLevel)
{
    switch (logLevel) {
        case 0: return kLevelDebug;
        case 1: return kLevelInfo;
        case 2: return kLevelWarn;
        case 3: return kLevelError;
        default: return kLevelInfo;
    }
}

static jsi::Value HostFunctionOpen(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    const jsi::Value *args,
    size_t count
)
{
    xlogger_SetLevel((TLogLevel)g_config.level);
    mars::xlog::appender_set_console_log(g_config.showConsoleLog);
    mars::xlog::XLogConfig config;
    config.mode_ = (g_config.appenderMode == 1) ? mars::xlog::kAppenderSync : mars::xlog::kAppenderAsync;
    config.logdir_ = g_config.logPath;
    config.nameprefix_ = g_config.namePrefix;
    config.pub_key_ = "";
    config.compress_mode_ = mars::xlog::kZlib;
    config.compress_level_ = 0;
    config.cachedir_ = "";
    config.cache_days_ = 0;
    mars::xlog::appender_open(config);
    facebook::react::bindNativeLogger(rt, [](const std::string &message, unsigned int logLevel) {
        TLogLevel tLogLevel = MapLogLevel(logLevel);
        WriteLog(tLogLevel, "RN", message.c_str());
    });

    auto promiseConstructor = rt.global().getPropertyAsFunction(rt, "Promise");
    auto executor = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "executor"),
        2,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue,
           const jsi::Value *executorArgs, size_t executorCount) -> jsi::Value {
            auto resolve = executorArgs[0].asObject(runtime).asFunction(runtime);
            resolve.call(runtime, jsi::Value::undefined());
            return jsi::Value::undefined();
        });

    return promiseConstructor.callAsConstructor(rt, executor);
}

static jsi::Value HostFunctionClose(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    const jsi::Value *args,
    size_t count
)
{
    rnoh::xlog::UninstallUncaughtSignalHandler();
    mars::xlog::appender_close();
    // 恢复框架默认日志:nativeLoggingHook → rnoh::nativeLogger → hilog
    facebook::react::bindNativeLogger(rt, rnoh::nativeLogger);

    auto promiseConstructor = rt.global().getPropertyAsFunction(rt, "Promise");
    auto executor = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "executor"),
        2,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue,
           const jsi::Value *executorArgs, size_t executorCount) -> jsi::Value {
            auto resolve = executorArgs[0].asObject(runtime).asFunction(runtime);
            resolve.call(runtime, jsi::Value::undefined());
            return jsi::Value::undefined();
        });

    return promiseConstructor.callAsConstructor(rt, executor);
}

static jsi::Value HostFunctionLog(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    const jsi::Value *args,
    size_t count
)
{
    int level = static_cast<int>(args[0].asNumber());
    auto tag = args[1].asString(rt).utf8(rt);
    auto message = args[2].asString(rt).utf8(rt);

    WriteLog((TLogLevel)level, tag.c_str(), message.c_str());

    auto promiseConstructor = rt.global().getPropertyAsFunction(rt, "Promise");
    auto executor = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "executor"),
        2,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue,
           const jsi::Value *executorArgs, size_t executorCount) -> jsi::Value {
            auto resolve = executorArgs[0].asObject(runtime).asFunction(runtime);
            resolve.call(runtime, jsi::Value::undefined());
            return jsi::Value::undefined();
        });

    return promiseConstructor.callAsConstructor(rt, executor);
}

static jsi::Value HostFunctionInstallCrashHandler(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    const jsi::Value *args,
    size_t count
)
{
    rnoh::xlog::InstallUncaughtSignalHandler();
    auto promiseConstructor = rt.global().getPropertyAsFunction(rt, "Promise");
    auto executor = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "executor"),
        2,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue,
           const jsi::Value *executorArgs, size_t executorCount) -> jsi::Value {
            auto resolve = executorArgs[0].asObject(runtime).asFunction(runtime);
            resolve.call(runtime, jsi::Value::undefined());
            return jsi::Value::undefined();
        });

    return promiseConstructor.callAsConstructor(rt, executor);
}

static jsi::Value HostFunctionUninstallCrashHandler(
    jsi::Runtime &rt,
    react::TurboModule &turboModule,
    const jsi::Value *args,
    size_t count
)
{
    rnoh::xlog::UninstallUncaughtSignalHandler();
    auto promiseConstructor = rt.global().getPropertyAsFunction(rt, "Promise");
    auto executor = jsi::Function::createFromHostFunction(
        rt,
        jsi::PropNameID::forAscii(rt, "executor"),
        2,
        [](jsi::Runtime &runtime, const jsi::Value &thisValue,
           const jsi::Value *executorArgs, size_t executorCount) -> jsi::Value {
            auto resolve = executorArgs[0].asObject(runtime).asFunction(runtime);
            resolve.call(runtime, jsi::Value::undefined());
            return jsi::Value::undefined();
        });

    return promiseConstructor.callAsConstructor(rt, executor);
}

RNXLogModule::RNXLogModule(const ArkTSTurboModule::Context ctx, const std::string name) : ArkTSTurboModule(ctx, name)
{
    methodMap_ = {
        {"open", MethodMetadata{0, HostFunctionOpen}},
        {"close", MethodMetadata{0, HostFunctionClose}},
        {"log", MethodMetadata{3, HostFunctionLog}},
        {"installUncaughtCrashHandler", MethodMetadata{0, HostFunctionInstallCrashHandler}},
        {"uninstallUncaughtCrashHandler", MethodMetadata{0, HostFunctionUninstallCrashHandler}},
    };
}

RNXLogModule::~RNXLogModule()
{
    rnoh::xlog::UninstallUncaughtSignalHandler();
    mars::xlog::appender_close();
}

} // namespace rnoh
