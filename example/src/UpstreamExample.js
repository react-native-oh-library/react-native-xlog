/** 上游 react-native-xlog example，适配当前示例的页面导航。 */
import React, {useEffect} from 'react';
import {ScrollView, StyleSheet, Text, TouchableOpacity, View} from 'react-native';
import XLog from '@react-native-ohos/react-native-xlog';

export default function UpstreamExample() {
  useEffect(() => {
    let active = true;
    const setup = (async () => {
      await XLog.open();
      if (active) {
        await XLog.installUncaughtCrashHandler();
      }
    })();
    setup.catch(error => console.error('XLog initialization failed:', error));
    return () => {
      active = false;
      // Wait for initialization before closing, even when leaving immediately.
      setup.catch(() => {}).then(async () => {
        await XLog.uninstallUncaughtCrashHandler();
        await XLog.close();
      }).catch(error => console.error('XLog cleanup failed:', error));
    };
  }, []);

  return (
    <ScrollView contentContainerStyle={styles.container}>
      <Text style={styles.welcome}>XLog Example</Text>
      <View style={styles.buttons}>
        <TouchableOpacity style={styles.btn} onPress={() => XLog.verbose('tag verbose', 'verbose message is here')}><Text>verbose</Text></TouchableOpacity>
        <TouchableOpacity style={styles.btn} onPress={() => XLog.debug('tag debug', 'debug message is here')}><Text>debug</Text></TouchableOpacity>
        <TouchableOpacity style={styles.btn} onPress={() => XLog.info('tag info', 'info message is here')}><Text>info</Text></TouchableOpacity>
        <TouchableOpacity style={styles.btn} onPress={() => XLog.warn('tag warn', 'warn message is here')}><Text>warn</Text></TouchableOpacity>
        <TouchableOpacity style={styles.btn} onPress={() => XLog.error('tag error', 'error message is here')}><Text>error</Text></TouchableOpacity>
        <TouchableOpacity style={styles.btn} onPress={() => XLog.fatal('tag fatal', 'fatal message is here')}><Text>fatal</Text></TouchableOpacity>
      </View>
      <Text style={styles.chapterText}>send console log</Text>
      <View style={styles.buttons}>
        <TouchableOpacity style={styles.btn} onPress={() => console.log('console log is here')}><Text>log</Text></TouchableOpacity>
        <TouchableOpacity style={styles.btn} onPress={() => console.warn('console warn is here')}><Text>warn</Text></TouchableOpacity>
        <TouchableOpacity style={styles.btn} onPress={() => console.error('console error is here')}><Text>error</Text></TouchableOpacity>
      </View>
      <Text style={styles.chapterText}>crash test</Text>
      <View style={styles.buttons}>
        <TouchableOpacity style={styles.btn} onPress={() => {throw new Error('crash test is here');}}><Text>crash</Text></TouchableOpacity>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {alignItems: 'center', paddingBottom: 30, backgroundColor: '#F5FCFF'},
  welcome: {fontSize: 20, textAlign: 'center', margin: 10},
  buttons: {width: '100%', flexWrap: 'wrap', flexDirection: 'row'},
  btn: {width: 80, height: 80, margin: 10, justifyContent: 'center', alignItems: 'center', backgroundColor: '#9ff'},
  chapterText: {marginTop: 30, width: '100%', textAlign: 'center'},
});
