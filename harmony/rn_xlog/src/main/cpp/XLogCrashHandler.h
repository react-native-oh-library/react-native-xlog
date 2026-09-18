/*
 * 对标 iOS XLogCrashHandler
 * 鸿蒙平台崩溃信号处理器
 */

#pragma once

namespace rnoh {
namespace xlog {

void InstallUncaughtSignalHandler(void);
void UninstallUncaughtSignalHandler(void);

} // namespace xlog
} // namespace rnoh
