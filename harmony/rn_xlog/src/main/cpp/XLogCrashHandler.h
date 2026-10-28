/*
 * 对标 iOS XLogCrashHandler
 * 鸿蒙平台崩溃信号处理器
 */

#pragma once

#include <string>

namespace rnoh {
namespace xlog {

void PrepareCrashLog(const std::string &logPath);
bool InstallUncaughtSignalHandler(void);
void UninstallUncaughtSignalHandler(void);

} // namespace xlog
} // namespace rnoh


