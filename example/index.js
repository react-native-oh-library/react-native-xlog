/**
 * example 入口:注册根组件。
 * 组件名与 harmony/entry/src/main/ets/pages/Index.ets 中的 appKey:"demo" 对应。
 */
import {AppRegistry} from 'react-native';
import {name as appName} from './app.json';
import App from './src/Home';

AppRegistry.registerComponent(appName, () => App);
