/*
 * Copyright (C) 2024 EngsSH. All rights reserved.
 * Licensed under the MIT License.
 */

#pragma once

#include "RNOH/ArkTSTurboModule.h"

#include <string>

namespace rnoh {

class JSI_EXPORT RNXLogModule : public ArkTSTurboModule {
public:
    ~RNXLogModule() override;
    RNXLogModule(const ArkTSTurboModule::Context ctx, const std::string name);

    // 对标 iOS +[XLogBridge registerWithLogPath:level:showConsoleLog:appenderMode:nameprefix:]
    // 由宿主在初始化时调用，配置 open() 所需参数（JS 层无需传入）
    static void Register(const std::string &logPath,
                         int level,
                         bool showConsoleLog,
                         int appenderMode,
                         const std::string &namePrefix);
};

} // namespace rnoh
