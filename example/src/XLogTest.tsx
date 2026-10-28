/**
 * react-native-xlog 鸿蒙手动测试页
 * 每个接口一个按钮,点击即调用一次,结果显示在底部日志区。
 */
import React, {useCallback, useRef, useState} from 'react';
import {
  Alert,
  TurboModuleRegistry,
  ScrollView,
  StatusBar,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
} from 'react-native';
import XLog from '@react-native-ohos/react-native-xlog';
import type {TurboModule} from 'react-native';

interface CrashTestModule extends TurboModule {
  crash(): void;
}

const TAG = 'rnt';

type LogLine = {
  time: string;
  text: string;
  ok: boolean;
};

const GROUPS: {
  title: string;
  buttons: {
    label: string;
    run: () => Promise<unknown>;
  }[];
}[] = [
  {
    title: '生命周期',
    buttons: [
      {label: 'open()', run: () => XLog.open()},
      {label: 'close()', run: () => XLog.close()},
    ],
  },
  {
    title: '日志写入',
    buttons: [
      {label: 'verbose()', run: () => XLog.verbose(TAG, 'RNT_VERBOSE_MARKER')},
      {label: 'debug()', run: () => XLog.debug(TAG, 'RNT_DEBUG_MARKER')},
      {label: 'info()', run: () => XLog.info(TAG, 'RNT_INFO_MARKER')},
      {label: 'warn()', run: () => XLog.warn(TAG, 'RNT_WARN_MARKER')},
      {label: 'error()', run: () => XLog.error(TAG, 'RNT_ERROR_MARKER')},
      {label: 'fatal()', run: () => XLog.fatal(TAG, 'RNT_FATAL_MARKER')},
    ],
  },
  {
    title: '崩溃捕获',
    buttons: [
      {label: 'installUncaughtCrashHandler()', run: () => XLog.installUncaughtCrashHandler()},
      {label: 'uninstallUncaughtCrashHandler()', run: () => XLog.uninstallUncaughtCrashHandler()},
      {
        label: '触发原生崩溃 (SIGABRT)',
        run: async () => {
          const crashTest = TurboModuleRegistry.get<CrashTestModule>('XLogCrashTest');
          if (!crashTest) throw new Error('测试原生模块不可用，请重新编译鸿蒙应用');
          Alert.alert('触发原生崩溃', '应用将立即退出，请确认已按测试需要安装或卸载处理器。', [
            {text: '取消', style: 'cancel'},
            {text: '触发崩溃', style: 'destructive', onPress: () => crashTest.crash()},
          ]);
        },
      },
    ],
  },
  {
    title: 'console 重定向',
    buttons: [
      {
        label: 'console.log(marker)',
        run: async () => {
          console.log('XLOG_CONSOLE_REDIRECT_OPEN_MARKER');
        },
      },
    ],
  },
];

export default function XLogTest({onBack}: {onBack?: () => void}) {
  const [lines, setLines] = useState<LogLine[]>([]);
  const busyRef = useRef(false);

  const append = useCallback((text: string, ok: boolean) => {
    const time = new Date().toTimeString().slice(0, 8);
    setLines(prev => [{time, text, ok}, ...prev].slice(0, 100));
  }, []);

  const press = useCallback(async (label: string, run: () => Promise<unknown>) => {
    if (busyRef.current) {
      return;
    }
    busyRef.current = true;
    const t0 = Date.now();
    try {
      await run();
      append(label + ' resolve (' + (Date.now() - t0) + 'ms)', true);
    } catch (e) {
      const msg = e instanceof Error ? e.message : String(e);
      append(label + ' 失败: ' + msg, false);
    }
    busyRef.current = false;
  }, [append]);

  return (
    <View style={styles.screen}>
      <View style={styles.header}>
        {onBack ? (
          <TouchableOpacity style={styles.backLink} onPress={onBack}>
            <Text style={styles.backLinkText}>{'< 返回首页'}</Text>
          </TouchableOpacity>
        ) : null}
        <Text style={styles.headerTitle}>XLog 手动测试页</Text>
      </View>
      <ScrollView style={styles.list}>
        {GROUPS.map(group => (
          <View key={group.title}>
            <Text style={styles.groupHeader}>{group.title}</Text>
            <View style={styles.btnWrap}>
              {group.buttons.map(btn => (
                <TouchableOpacity
                  key={btn.label}
                  style={styles.btn}
                  activeOpacity={0.6}
                  onPress={() => press(btn.label, btn.run)}>
                  <Text style={styles.btnText}>{btn.label}</Text>
                </TouchableOpacity>
              ))}
            </View>
          </View>
        ))}
        <View style={styles.logBox}>
          <Text style={styles.logTitle}>调用日志(最新在上)</Text>
          {lines.length === 0 ? (
            <Text style={styles.logEmpty}>尚未调用任何接口</Text>
          ) : (
            lines.map((line, idx) => (
              <Text key={line.time + idx} style={line.ok ? styles.logOk : styles.logFail}>
                {line.time + '  ' + line.text}
              </Text>
            ))
          )}
        </View>
      </ScrollView>
    </View>
  );
}

const styles = StyleSheet.create({
  screen: {
    flex: 1,
    backgroundColor: '#F5FCFF',
  },
  header: {
    paddingTop: (StatusBar.currentHeight || 0) + 8,
    paddingHorizontal: 16,
    paddingBottom: 8,
    backgroundColor: '#FFFFFF',
    borderBottomWidth: StyleSheet.hairlineWidth,
    borderBottomColor: '#C7C7CC',
  },
  backLink: {
    alignSelf: 'flex-start',
    paddingVertical: 2,
    marginBottom: 4,
  },
  backLinkText: {
    fontSize: 14,
    color: '#007AFF',
  },
  headerTitle: {
    fontSize: 18,
    fontWeight: '600',
  },
  list: {
    flex: 1,
  },
  groupHeader: {
    fontSize: 13,
    fontWeight: '600',
    color: '#3C3C43',
    marginTop: 14,
    marginLeft: 16,
    marginBottom: 4,
  },
  btnWrap: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    paddingHorizontal: 12,
  },
  btn: {
    paddingHorizontal: 12,
    paddingVertical: 8,
    borderRadius: 8,
    backgroundColor: '#007AFF',
    marginRight: 8,
    marginTop: 4,
  },
  btnText: {
    color: '#FFFFFF',
    fontSize: 13,
    fontWeight: '500',
  },
  logBox: {
    margin: 12,
    marginTop: 16,
    padding: 12,
    borderRadius: 8,
    backgroundColor: '#FFFFFF',
    marginBottom: 24,
  },
  logTitle: {
    fontSize: 14,
    fontWeight: '600',
    marginBottom: 6,
  },
  logEmpty: {
    fontSize: 12,
    color: '#8E8E93',
  },
  logOk: {
    fontSize: 12,
    color: '#1B7F3B',
    marginTop: 3,
  },
  logFail: {
    fontSize: 12,
    color: '#C53030',
    marginTop: 3,
  },
});


