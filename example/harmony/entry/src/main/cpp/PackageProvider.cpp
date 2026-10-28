#include "RNOH/PackageProvider.h"
#include "generated/RNOHGeneratedPackage.h"
#include "RNXLogPackage.h"
#include "CrashTestPackage.h"
#include <sys/stat.h>

using namespace rnoh;
std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx) {
    // 宿主侧注册 xlog 配置(对标 iOS/Android 宿主侧 init/registerWithLogPath:...)。
    // g_config 为进程级配置,须在 JS 调 open() 之前调用;
    // 目录不存在时先创建(mars appender_open 不自动建目录),已存在返回 EEXIST 无影响。
    mkdir("/data/storage/el2/base/haps/entry/cache/xlog_test", 0755);
    RNXLogPackage::registerXLog(
        "/data/storage/el2/base/haps/entry/cache/xlog_test",
        0,      // level = kLevelAll,全级别放行
        true,   // showConsoleLog:日志同步回显到 hilog(内容验证的主通道)
        1,      // 测试使用同步模式，降低崩溃时日志尚未写入的概率
        "Test"); // 日志文件名前缀,产物为 Test_YYYYMMDD.xlog
    return { 
        std::make_shared<RNOHGeneratedPackage>(ctx),
        std::make_shared<RNXLogPackage>(ctx),
        std::make_shared<CrashTestPackage>(ctx),
    };
}

