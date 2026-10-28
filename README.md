> 文档模板:v0.4.2

<p align="center">
  <h1 align="center"> <code>react-native-xlog</code> </h1>
</p>
本项目基于 [react-native-xlog](https://www.npmjs.com/package/react-native-xlog) 开发。

该三方库已适配 React Native OpenHarmony(RNOH),支持直接从 npm 下载,包名为:`@react-native-ohos/react-native-xlog`(JS 别名 `react-native-xlog`),版本所属关系如下:

| 三方库名称 | 三方库版本(npm地址) | 发布信息 | 支持RN版本 | Autolink | 编译API版本 | 社区基线版本 | 源码地址 |
| - | - | - | - | - | - | - | - |
| @react-native-ohos/react-native-xlog | [~ 0.5.1 (开发中)](https://www.npmjs.com/package/@react-native-ohos/react-native-xlog) | [GitHub Releases](https://github.com/react-native-oh-library/react-native-xlog/releases) | 0.72.* / 0.77.* / 0.82.* / 0.84.* | 是 | API12+ | 0.6.1 | [master](https://github.com/react-native-oh-library/react-native-xlog) |

## 简介

react-native-xlog 是腾讯 [mars xlog](https://github.com/Tencent/mars) 高性能日志库的 RN 封装:JS 侧管理 xlog 生命周期(open/close),提供六级日志接口(verbose/debug/info/warn/error/fatal),并支持将 native 崩溃以 fatal 级别写入 xlog。

本库为其提供 React Native 鸿蒙(HarmonyOS)适配,基于 **TurboModule** 实现。支持:

- 生命周期:open / close(宿主侧可先经 registerXLog 注册配置)
- 六级日志写入(xlogger_Write 直写)
- 崩溃捕获:安装处理器后,应用崩溃信息写入 xlog
- console.log 重定向:open 后 JS console 输出写入 xlog,close 后恢复框架默认(hilog)

## Link

| | 是否支持autolink | RN框架版本 |
| - | - | - |
| ~0.5.1 | 是 | 0.72.* / 0.77.* / 0.82.* / 0.84.* |

使用AutoLink的工程需要根据该文档配置,Autolink框架指导文档:https://gitcode.com/CPF-RN/ohos_react_native/blob/master/docs/zh-cn/Autolinking.md

如您使用的版本支持 Autolink,并且工程已接入 Autolink,可跳过ManualLink配置。
<details>
  <summary>ManualLink: 此步骤为手动配置原生依赖项的指导</summary>

首先需要使用 DevEco Studio 打开项目里的 HarmonyOS 工程 `harmony`。

### 1. Overrides RN SDK

为了让工程依赖同一个版本的 RN SDK,需要在工程根目录的 `oh-package.json5` 添加 overrides 字段,指向工程需要使用的 RN SDK 版本。替换的版本既可以是一个具体的版本号,也可以是一个模糊版本,还可以是本地存在的 HAR 包或源码目录。

关于该字段的作用请阅读[官方说明](https://developer.huawei.com/consumer/cn/doc/harmonyos-guides-V5/ide-oh-package-json5-V5#zh-cn_topic_0000001792256137_overrides)

```json
{
  "overrides": {
    "@rnoh/react-native-openharmony": "~0.84.2" // 0.84 工程;其他框架版本使用对应的 RNOH 版本
    // "@rnoh/react-native-openharmony" : "./react_native_openharmony.har" // 指向本地 har 包的路径
    // "@rnoh/react-native-openharmony" : "./react_native_openharmony" // 指向源码路径
  }
}
```

### 2. 引入原生端代码

目前有两种方法:

1. 通过 har 包引入(在 IDE 完善相关功能后该方法会被遗弃,目前首选此方法);
2. 直接链接源码。

方法一:通过 har 包引入(推荐)

> [!TIP] har 包位于三方库安装路径的 `harmony` 文件夹下。

打开 `entry/oh-package.json5`,添加以下依赖

```json
"dependencies": {
    "@rnoh/react-native-openharmony": "file:../react_native_openharmony",
    "@react-native-ohos/react-native-xlog": "file:../../node_modules/@react-native-ohos/react-native-xlog/harmony/rn_xlog.har"
  }
```

点击右上角的 `sync` 按钮

或者在终端执行:

```bash
cd entry
ohpm install
```

方法二:直接链接源码

> [!TIP] 如需使用直接链接源码,请参考[直接链接源码说明](https://gitcode.com/CPF-RN/usage-docs/blob/master/zh-cn/link-source-code.md)

### 3. 配置 CMakeLists 和引入 RNXLogPackage

> 若工程已接入 RNOH AutoLink,且本库的 `harmony.autolinking` 配置生效(RNXLogPackage / rnoh_xlog),则 CMake 与 PackageProvider 注册可由 AutoLink 自动生成,可跳过本节手动配置。

打开 `entry/src/main/cpp/CMakeLists.txt`,添加:

```diff
project(rnapp)
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(RNOH_APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(NODE_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../node_modules")
set(OH_MODULES "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules")
set(RNOH_CPP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../oh_modules/@rnoh/react-native-openharmony/src/main/cpp")
set(LOG_VERBOSITY_LEVEL 1)
set(CMAKE_ASM_FLAGS "-Wno-error=unused-command-line-argument -Qunused-arguments")
set(CMAKE_CXX_FLAGS "-fstack-protector-strong -Wl,-z,relro,-z,now,-z,noexecstack -s -fPIE -pie")
set(WITH_HITRACE_SYSTRACE 1) # for other CMakeLists.txt files to use
add_compile_definitions(WITH_HITRACE_SYSTRACE)

add_subdirectory("${RNOH_CPP_DIR}" ./rn)
+ add_subdirectory("${OH_MODULES}/@react-native-ohos/react-native-xlog/src/main/cpp" ./rn_xlog)

# RNOH_BEGIN: manual_package_linking_1
# RNOH_END: manual_package_linking_1

file(GLOB GENERATED_CPP_FILES "./generated/*.cpp")

add_library(rnoh_app SHARED
    ${GENERATED_CPP_FILES}
    "./PackageProvider.cpp"
    "${RNOH_CPP_DIR}/RNOHAppNapiBridge.cpp"
)
target_link_libraries(rnoh_app PUBLIC rnoh)
+ target_link_libraries(rnoh_app PUBLIC rnoh_xlog)
```

打开 `entry/src/main/cpp/PackageProvider.cpp`,添加:

```diff
  #include "RNOH/PackageProvider.h"
+ #include "RNXLogPackage.h"

  using namespace rnoh;

  std::vector<std::shared_ptr<Package>> PackageProvider::getPackages(Package::Context ctx) {
      return {
+       std::make_shared<RNXLogPackage>(ctx),
      };
  }
```

### 4. 在 ArkTs 侧引入 RNXLogPackage

打开 `entry/src/main/ets/RNPackagesFactory.ts`(.ets),添加:

```diff
+ import { RNXLogPackage } from '@react-native-ohos/react-native-xlog';

  export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
    return [
+     new RNXLogPackage(ctx),
    ];
  }
```
</details>

### 配置日志

在 `entry/src/main/cpp/PackageProvider.cpp` 中，可在 `getPackages()` 内、调用 `XLog.open()` 之前配置日志。示例工程使用以下配置：

```cpp
#include <sys/stat.h>
#include "RNXLogPackage.h"

// 在 PackageProvider::getPackages() 中调用
mkdir("/data/storage/el2/base/haps/entry/cache/xlog_test", 0755);
RNXLogPackage::registerXLog(
    "/data/storage/el2/base/haps/entry/cache/xlog_test", // 应用沙箱中的日志目录
    0,      // 记录所有日志级别
    true,   // 同时输出到 hilog
    1,      // 同步模式；0 为异步模式
    "Test"); // 日志文件名前缀，例如 Test_YYYYMMDD.xlog
```

请根据应用调整日志目录，并确保目录存在且可写。该配置适用于自动链接和手动链接；不注册时使用默认配置。

### 运行

点击右上角的 `sync` 按钮

或者在终端执行:

```bash
cd entry
ohpm install
```

然后编译、运行即可。

## 约束与限制

### 兼容性

要使用此库，需要使用正确的 React-Native 和 RNOH 版本。另外，还需要使用配套的 DevEco Studio 和 手机 ROM。

在以下版本验证通过：

1. RNOH: 0.84.2; SDK: HarmonyOS 6.0.1 Release SDK; IDE: DevEco Studio 6.0.1 Release; ROM: 6.0.0.120 SP7;
2. RNOH: 0.82.1; SDK: HarmonyOS API Version 21 Release SDK; IDE: DevEco Studio 6.0.1.260; ROM: 6.0.0.130;
3. RNOH：0.77.18-1; SDK：HarmonyOS 6.0.2.57 (API Version 22); IDE：DevEco Studio 6.0.1.112; ROM：5.1.1.45;
4. RNOH：0.72.133-1; SDK：HarmonyOS 6.0.2.57 (API Version 22); IDE：DevEco Studio 6.0.1.112; ROM：5.0.0.102;

### 权限要求

无

### 编译运行API要求

> [!TIP] 当前三方库所有版本均已实现版本隔离,支持在 `API12+` 工程编译,及 `API12+` ROM运行。

## 使用示例

下面的代码展示了这个库的基本使用场景:

> [!WARNING] 使用时 import 的库名不变,JS 侧通过 `react-native-xlog` 导入(Autolink 工程用别名导入;har 手动链接工程用 `@react-native-ohos/react-native-xlog` 导入,两者等价)。

```javascript
import XLog from 'react-native-xlog';

// 宿主侧可选:在 PackageProvider.cpp 中先注册配置(不注册时使用默认配置:
// logPath=应用沙箱 cache、level=All、showConsoleLog=true、异步 appender、文件名前缀 Test)
// RNXLogPackage::registerXLog("/data/storage/el2/base/haps/entry/cache/xlog", 0, true, 0, "Test");

await XLog.open();
XLog.info('tag', 'hello xlog');
XLog.error('tag', 'something failed');
await XLog.close();
```

## 使用说明

**生命周期管理**

```javascript
import XLog from 'react-native-xlog';

// 开启 xlog(读取宿主 registerXLog 注册的配置;未注册时用默认配置)
await XLog.open();

// 关闭 xlog(会同步卸载已安装的崩溃处理器并恢复 console 默认行为)
await XLog.close();
```

**六级日志写入**

```javascript
XLog.verbose('tag', '...');
XLog.debug('tag', '...');
XLog.info('tag', '...');
XLog.warn('tag', '...');
XLog.error('tag', '...');
XLog.fatal('tag', '...');
```

**崩溃捕获**

```javascript
// 先打开日志，再安装崩溃处理器
await XLog.open();
// 安装崩溃处理器
await XLog.installUncaughtCrashHandler();

// 卸载(close() 内部也会卸载)
await XLog.uninstallUncaughtCrashHandler();

// 鸿蒙侧在崩溃后重新启动并调用 open()，将信号编号写入日志
```

**console 重定向**

```javascript
// open 后,JS 的 console.log/info/warn/error 输出会写入 xlog(hilog 中以 xlog 格式回显,tag=RN);
// close 后恢复为框架默认(hilog)
await XLog.open();
console.log('这条会进 xlog');
await XLog.close();
console.log('这条回到 hilog 默认输出');
```

## 接口说明

> [!TIP] "Platform"列表示该属性在原三方库上支持的平台。

> [!TIP] "HarmonyOS Support"列为 yes 表示 HarmonyOS 平台支持该属性;no 则表示不支持;partially 表示部分支持。使用方法跨平台一致,效果对标 iOS 的效果。

本库对外暴露 `XLog` 模块(TurboModule `XLogBridge` 的 JS 封装)。本库无导出属性/常量,接口全部为方法,说明如下:

### 方法

| Name | Description | Type | Platform | HarmonyOS Support |
| - | - | - | - | - |
| open | 开启 xlog(读取宿主注册的配置;未注册时用默认配置),并把 JS console 输出重定向进 xlog | `() => Promise<void>` | iOS / Android | yes |
| close | 关闭 xlog(卸载崩溃处理器、恢复 console 默认输出) | `() => Promise<void>` | iOS / Android | yes |
| verbose | 以 verbose 级别写一条日志 | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| debug | 以 debug 级别写一条日志 | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| info | 以 info 级别写一条日志 | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| warn | 以 warn 级别写一条日志 | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| error | 以 error 级别写一条日志 | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| fatal | 以 fatal 级别写一条日志 | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| installUncaughtCrashHandler | 安装崩溃处理器,捕获应用崩溃写入 xlog | `() => Promise<void>` | iOS / Android | yes |
| uninstallUncaughtCrashHandler | 卸载崩溃信号处理器 | `() => Promise<void>` | iOS / Android | yes |

## 遗留问题

无

## 其他

无

## 目录结构

````text
/xlog                                    # 项目根目录
├── harmony                              # 鸿蒙适配代码
│   ├── rn_xlog.har                      # har 包
│   └── rn_xlog                          # 鸿蒙适配核心代码
│       ├── Index.ets                    # 鸿蒙适配代码入口
│       ├── ts.ets                       # ArkTS 侧类型导出入口
│       └── src/main
│           ├── ets
│           │   ├── RNXLogPackage.ets    # ArkTS Package(TurboModule 注册枢纽)
│           │   ├── RNXLogModule.ets     # ArkTS TurboModule(薄包装层,逻辑在 cpp)
│           │   └── generated/           # codegen 生成的 ArkTS 代码
│           └── cpp
│               ├── CMakeLists.txt       # C++ 侧构建配置(目标 rnoh_xlog)
│               ├── RNXLogPackage.h      # 覆盖 codegen 基类的 Package 工厂
│               ├── RNXLogModule.cpp/.h  # TurboModule cpp 实现(open/close/log/crash)
│               ├── XLogCrashHandler.cpp/.h  # 崩溃信号处理器
│               ├── XLogNativeLogger.h   # bindNativeLogger 跨版本声明
│               ├── mars/                # mars xlog 头文件
│               └── generated/           # codegen 生成的 cpp 代码
├── example                              # 示例工程(upstream 示例 + rnt 手动测试页)
│   ├── src/Home.js                      # 首页:upstream / rnt 双入口
│   ├── src/UpstreamExample.js           # 上游示例页面
│   ├── src/XLogTest.tsx                 # rnt:手动测试页
│   ├── src/types/                       # TS 类型垫片
│   └── harmony                          # DevEco 工程
├── buildEnv.sh                          # 构建环境变量
├── index.js                             # JS 入口(10 个导出接口)
├── README.md                            # 中文文档
└── README_en.md                         # 英文文档
````

## 贡献代码

使用过程中发现任何问题都可以提交 [Issue](https://github.com/react-native-oh-library/react-native-xlog/issues),当然,也非常欢迎提交 [PR](https://github.com/react-native-oh-library/react-native-xlog/pulls)。

## 开源协议

本项目基于 [MIT License](https://github.com/EngsShi/react-native-xlog/blob/master/LICENSE),请自由地享受和参与开源。

