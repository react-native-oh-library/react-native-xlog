/**
 * react-native-xlog TS 类型垫片(example 本地)。
 * 库本体为 Flow JS(main=index.js),供 tsc/编辑器提示用;
 * 运行时以库 index.js 实际导出为准。
 */
declare module '@react-native-ohos/react-native-xlog' {
  const XLog: {
    open(): Promise<void>;
    close(): Promise<void>;
    installUncaughtCrashHandler(): Promise<void>;
    uninstallUncaughtCrashHandler(): Promise<void>;
    verbose(tag: string, message: string): Promise<void>;
    debug(tag: string, message: string): Promise<void>;
    info(tag: string, message: string): Promise<void>;
    warn(tag: string, message: string): Promise<void>;
    error(tag: string, message: string): Promise<void>;
    fatal(tag: string, message: string): Promise<void>;
  };
  export default XLog;
}
