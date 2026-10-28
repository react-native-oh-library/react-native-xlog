#pragma once

#include "RNOH/Package.h"
#include "RNOH/ArkTSTurboModule.h"
#include <cstdlib>

namespace rnoh {
class CrashTestModule : public ArkTSTurboModule {
public:
    CrashTestModule(Context ctx, const std::string &name) : ArkTSTurboModule(ctx, name) {
        methodMap_ = {{"crash", MethodMetadata{0, [](facebook::jsi::Runtime &rt,
            facebook::react::TurboModule &, const facebook::jsi::Value *, size_t) -> facebook::jsi::Value {
            std::abort();
        }}}};
    }
};
class CrashTestFactory : public TurboModuleFactoryDelegate {
public:
    SharedTurboModule createTurboModule(Context ctx, const std::string &name) const override {
        if (name == "XLogCrashTest") return std::make_shared<CrashTestModule>(ctx, name);
        return nullptr;
    }
};
class CrashTestPackage : public Package {
public:
    using Package::Package;
    std::unique_ptr<TurboModuleFactoryDelegate> createTurboModuleFactoryDelegate() override {
        return std::make_unique<CrashTestFactory>();
    }
};
}
