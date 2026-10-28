import { CrashTestPackage } from './CrashTestPackage';
import { RNXLogPackage } from '@react-native-ohos/react-native-xlog';
import { RNPackageContext, RNPackage } from '@rnoh/react-native-openharmony/ts';
export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
  return [
    new RNXLogPackage(ctx),
    new CrashTestPackage(ctx)
  ];
}
