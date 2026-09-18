/*
 * ArkTS TurboModule 薄包装层
 * 实际逻辑在 C++ RNXLogModule 中实现
 */

import { UITurboModule, UITurboModuleContext } from '@rnoh/react-native-openharmony/ts';
import { TM } from "./generated/ts"

export class RNXLogModule extends UITurboModule {
  static NAME = "XLogBridge"

  constructor(ctx: UITurboModuleContext) {
    super(ctx);
  }
}
