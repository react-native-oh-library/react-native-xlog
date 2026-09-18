/*
 * ArkTS Package 注册
 * 创建 TurboModulesFactory，将 "XLogBridge" 映射到 RNXLogModule
 */

import { TurboModulesFactory } from '@rnoh/react-native-openharmony/ts';
import type {
  TurboModule, TurboModuleContext
} from '@rnoh/react-native-openharmony/ts';
import { ComponentBuilderContext, RNOHPackage } from '@rnoh/react-native-openharmony'
import { RNXLogModule } from "./RNXLogModule"
import { TM } from "./generated/ts";

class XLogTurboModulesFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    if (name === TM.XLogBridge.NAME) {
      return new RNXLogModule(this.ctx);
    }
    return null;
  }

  hasTurboModule(name: string): boolean {
    return name === TM.XLogBridge.NAME;
  }
}

export class RNXLogPackage extends RNOHPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    return new XLogTurboModulesFactory(ctx);
  }
}
