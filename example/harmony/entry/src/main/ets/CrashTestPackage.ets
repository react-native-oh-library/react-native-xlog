import { RNOHPackage } from '@rnoh/react-native-openharmony';
import {
  TurboModulesFactory,
  UITurboModule,
  UITurboModuleContext
} from '@rnoh/react-native-openharmony/ts';
import type { TurboModule, TurboModuleContext } from '@rnoh/react-native-openharmony/ts';

class CrashTestModule extends UITurboModule {
  constructor(ctx: UITurboModuleContext) {
    super(ctx);
  }
}

class CrashTestFactory extends TurboModulesFactory {
  createTurboModule(name: string): TurboModule | null {
    return name === 'XLogCrashTest' ? new CrashTestModule(this.ctx) : null;
  }

  hasTurboModule(name: string): boolean {
    return name === 'XLogCrashTest';
  }
}

export class CrashTestPackage extends RNOHPackage {
  createTurboModulesFactory(ctx: TurboModuleContext): TurboModulesFactory {
    return new CrashTestFactory(ctx);
  }
}
