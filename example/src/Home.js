/**
 * example 首页:upstream / rnt 双入口。
 * upstream = 上游(原库)demo。
 * rnt      = 鸿蒙全量测试页(覆盖全部对外 JS 接口与属性)。
 */
import React, {useEffect, useState} from 'react';
import {
  BackHandler,
  StatusBar,
  StyleSheet,
  Text,
  TouchableOpacity,
  View,
} from 'react-native';
import XLogTest from './XLogTest';
import UpstreamExample from './UpstreamExample';

export default function Home() {
  const [page, setPage] = useState('home');

  // 物理返回键:子页面 → 首页;首页 → 交给系统默认(退出)
  useEffect(() => {
    const backSub = BackHandler.addEventListener('hardwareBackPress', () => {
      if (page !== 'home') {
        setPage('home');
        return true;
      }
      return false;
    });
    return () => backSub.remove();
  }, [page]);

  if (page === 'upstream') {
    return (
      <View style={styles.screen}>
        <TouchableOpacity style={styles.backButton} onPress={() => setPage('home')}>
          <Text style={styles.backText}>返回首页</Text>
        </TouchableOpacity>
        <UpstreamExample />
      </View>
    );
  }

  if (page === 'rnt') {
    return (
      <View style={styles.screen}>
        <XLogTest onBack={() => setPage('home')} />
      </View>
    );
  }

  return (
    <View style={styles.container}>
      <View style={styles.home}>
        <Text style={styles.title}>react-native-xlog</Text>
        <TouchableOpacity
          style={styles.button}
          onPress={() => setPage('upstream')}
          accessibilityLabel="upstream">
          <Text style={styles.buttonText}>upstream</Text>
        </TouchableOpacity>
        <TouchableOpacity
          style={styles.button}
          onPress={() => setPage('rnt')}
          accessibilityLabel="rnt">
          <Text style={styles.buttonText}>rnt</Text>
        </TouchableOpacity>
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  screen: {
    flex: 1,
    backgroundColor: '#F5FCFF',
  },
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
    backgroundColor: '#F5FCFF',
  },
  home: {
    alignItems: 'center',
  },
  title: {
    fontSize: 20,
    fontWeight: '600',
    marginBottom: 32,
  },
  button: {
    width: 200,
    paddingVertical: 12,
    alignItems: 'center',
    borderRadius: 8,
    backgroundColor: '#007AFF',
    marginBottom: 16,
  },
  buttonText: {
    color: '#FFFFFF',
    fontSize: 16,
    fontWeight: '600',
  },
  backButton: {
    padding: 8,
    margin: 8,
    marginTop: (StatusBar.currentHeight || 0) + 12,
    alignSelf: 'flex-start',
    borderWidth: 1,
    borderColor: 'white',
    borderRadius: 3,
    backgroundColor: 'grey',
  },
  backText: {
    color: '#FFFFFF',
  },
});

