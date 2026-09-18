/*
 * XLogNativeLogger.h
 * RN 日志重定向的跨版本兼容声明(适配 RNOH 0.72 / 0.77 / 0.82 / 0.84)
 *
 * bindNativeLogger 在 RNOH 0.72~0.82 声明于
 *   third-party/rn/ReactCommon/jsiexecutor/jsireact/JSIExecutor.h
 * 0.84 起搬至
 *   third-party/rn/ReactCommon/jsitooling/react/runtime/JSRuntimeBindings.h
 * 四个版本签名逐字一致,实现均编译在框架 so 中(0.72~0.82 在 JSIExecutor.cpp,
 * 0.84 在 jsitooling 的 JSRuntimeBindings.cpp),链接 rnoh 即可解析,
 * 故此处直接自声明,不再 include 版本敏感的框架头。
 * 若未来版本变更签名,将在链接期报 undefined symbol,可及时发现。
 */
#pragma once

#include <jsi/jsi.h>
#include <functional>
#include <string>

namespace facebook::react {
using Logger = std::function<void(const std::string &message, unsigned int logLevel)>;
void bindNativeLogger(jsi::Runtime &runtime, Logger logger);
} // namespace facebook::react

namespace rnoh {
// RNOH 框架默认 logger(四版本同位于 RNOH/NativeLogger.h,输出 hilog "#RNOH_JS"),
// 用于 close 时恢复框架默认日志行为
void nativeLogger(const std::string &message, unsigned int logLevel);
} // namespace rnoh
