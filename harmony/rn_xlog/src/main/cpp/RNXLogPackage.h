/*
 * 自定义 Package，覆盖 BaseReactNativeXlogPackage 的工厂方法
 * 使 TurboModule 名称 "XLogBridge" 指向自定义的 RNXLogModule
 */

#pragma once

#include "generated/RNOH/generated/BaseReactNativeXlogPackage.h"
#include "RNXLogModule.h"

namespace rnoh {

class RNXLogModuleTurboModuleFactoryDelegate : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "XLogBridge") {
            return std::make_shared<RNXLogModule>(ctx, name);
        }
        return nullptr;
    }
};

class RNXLogPackage : public BaseReactNativeXlogPackage {
    using Super = BaseReactNativeXlogPackage;
    using Super::Super;

public:
    static void registerXLog(const std::string &logPath,
                             int level = 0,
                             bool showConsoleLog = true,
                             int appenderMode = 0,
                             const std::string &namePrefix = "Test") {
        RNXLogModule::Register(logPath, level, showConsoleLog, appenderMode, namePrefix);
    }

private:
    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override {
        return std::make_unique<RNXLogModuleTurboModuleFactoryDelegate>();
    }
};

} // namespace rnoh
