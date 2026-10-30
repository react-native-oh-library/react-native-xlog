> Document Template: v0.4.2

<p align="center">
  <h1 align="center"> <code>react-native-xlog</code> </h1>
</p>

This project is based on [react-native-xlog](https://www.npmjs.com/package/react-native-xlog).

This third-party library has been migrated to Gitcode and is now available for direct download from npm, the new package name is: `@react-native-ohos/react-native-xlog`, the version correspondence details are as follows:

| Name | Version(Npm Address) | Release Information | Supported RN Version | Supported Autolink | Compile API Version | Community Baseline Version | Source code address |
| - | - | - | - | - | - | - | - |
| @react-native-ohos/react-native-xlog | [~ 0.5.1 (In development)](https://www.npmjs.com/package/@react-native-ohos/react-native-xlog) | [GitHub Releases](https://github.com/react-native-oh-library/react-native-xlog/releases) | 0.72.* / 0.77.* / 0.82.* / 0.84.* | Yes | API12+ | 0.6.1 | [master](https://github.com/react-native-oh-library/react-native-xlog) |

## Introduction

react-native-xlog is a React Native wrapper of Tencent's high-performance [mars xlog](https://github.com/Tencent/mars) logging library: the JS side manages the xlog lifecycle (open/close) and provides six log levels (verbose/debug/info/warn/error/fatal), plus writing native crashes to xlog at the fatal level.

This library provides the React Native HarmonyOS adaptation, implemented using a **TurboModule**. The supported features are:

- Lifecycle: open / close (the host app may register configuration via registerXLog first)
- Six log levels (written directly with xlogger_Write)
- Crash capture: after installing the handler, app crash info is written to xlog
- console.log redirection: after open, JS console output goes to xlog; after close, the framework default (hilog) is restored

## Installation

Go to the project directory and execute the following instruction:

**npm**

```bash
npm install @react-native-ohos/react-native-xlog
```

**yarn**

```bash
yarn add @react-native-ohos/react-native-xlog
```

## Link

| | Supported Autolink | Supported RN Version |
| - | - | - |
| ~0.5.1 | Yes | 0.72.* / 0.77.* / 0.82.* / 0.84.* |

Projects using AutoLink need to be configured according to this document, AutoLink framework guide: https://gitcode.com/CPF-RN/ohos_react_native/blob/master/docs/en/Autolinking.md

If the version you are using supports Autolink and the project has integrated Autolink, you can skip the ManualLink configuration.

<details>
  <summary>ManualLink: This step provides guidance for manually configuring native dependencies.</summary>

Open the `harmony` directory of the OpenHarmony project in DevEco Studio.

### 1. Overrides RN SDK

To ensure the project relies on the same version of the RN SDK, you need to add an `overrides` field in the project's root `oh-package.json5` file, specifying the RN SDK version to be used. The replacement version can be a specific version number, a semver range, or a locally available HAR package or source directory.

For more information about the purpose of this field, please refer to the [official documentation](https://developer.huawei.com/consumer/en/doc/harmonyos-guides-V5/ide-oh-package-json5-V5#en-us_topic_0000001792256137_overrides).

```json5
{
  "overrides": {
    "@rnoh/react-native-openharmony": "~0.84.2" // 0.84 project; use the matching RNOH version for other framework versions
    // "@rnoh/react-native-openharmony" : "./react_native_openharmony.har" // path to a local har package
    // "@rnoh/react-native-openharmony" : "./react_native_openharmony" // path to source code
  }
}
```

### 2. Introducing Native Code

Currently, two methods are available:

- Use the HAR file.
- Directly link to the source code.

Method 1 (recommended): Use the HAR file.

> [!TIP] The HAR file is stored in the `harmony` directory in the installation path of the third-party library.

Open `entry/oh-package.json5` file and add the following dependencies:

```json
"dependencies": {
    "@rnoh/react-native-openharmony": "file:../react_native_openharmony",
    "@react-native-ohos/react-native-xlog": "file:../../node_modules/@react-native-ohos/react-native-xlog/harmony/rn_xlog.har"
  }
```

Click the `sync` button in the upper right corner.

Alternatively, run the following instruction on the terminal:

```bash
cd entry
ohpm install
```

Method 2: Directly link to the source code.

> [!TIP] For details, see [Directly Linking Source Code](https://gitcode.com/CPF-RN/usage-docs/blob/master/en/link-source-code.md).

### 3. Configuring CMakeLists and Introducing RNXLogPackage

> If the project has integrated RNOH AutoLink and the `harmony.autolinking` configuration (RNXLogPackage / rnoh_xlog) takes effect, the CMake and PackageProvider registration can be generated automatically by AutoLink, and this section can be skipped.

Open `entry/src/main/cpp/CMakeLists.txt` and add the following code:

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

Open `entry/src/main/cpp/PackageProvider.cpp` and add:

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

### 4. Introducing RNXLogPackage to ArkTS

Open `entry/src/main/ets/RNPackagesFactory.ts` (or `.ets`) and add:

```diff
+ import { RNXLogPackage } from '@react-native-ohos/react-native-xlog';

  export function createRNPackages(ctx: RNPackageContext): RNPackage[] {
    return [
+     new RNXLogPackage(ctx),
    ];
  }
```
</details>

### Configure logging

In `entry/src/main/cpp/PackageProvider.cpp`, optionally configure logging inside `getPackages()` before calling `XLog.open()`. The example project uses:

```cpp
#include <sys/stat.h>
#include "RNXLogPackage.h"

// Call inside PackageProvider::getPackages()
mkdir("/data/storage/el2/base/haps/entry/cache/xlog_test", 0755);
RNXLogPackage::registerXLog(
    "/data/storage/el2/base/haps/entry/cache/xlog_test", // Log directory in the app sandbox
    0,      // Enable all log levels
    true,   // Also output logs to hilog
    1,      // Synchronous mode; 0 selects asynchronous mode
    "Test"); // File prefix, e.g. Test_YYYYMMDD.xlog
```

Adjust the directory for your app and ensure it exists and is writable. This configuration applies to both automatic and manual linking; defaults are used if it is not registered.

## Running

Click the `sync` button in the upper right corner.

Alternatively, run the following instruction on the terminal:

```bash
cd entry
ohpm install
```

Then build and run the code.


## Constraints

### Compatibility

To use this library, you need the correct React-Native and RNOH versions, plus the matching DevEco Studio and phone ROM.

The following environments have been verified:

1. RNOH: 0.84.2; SDK: HarmonyOS 6.0.1 Release SDK; IDE: DevEco Studio 6.0.1 Release; ROM: 6.0.0.120 SP7.
2. RNOH: 0.82.1; SDK: HarmonyOS API Version 21 Release SDK; IDE: DevEco Studio 6.0.1.260; ROM: 6.0.0.130.
3. RNOH: 0.77.18-1; SDK: HarmonyOS 6.0.2.57 (API Version 22); IDE: DevEco Studio 6.0.1.112; ROM: 5.1.1.45.
4. RNOH: 0.72.133-1; SDK: HarmonyOS 6.0.2.57 (API Version 22); IDE: DevEco Studio 6.0.1.112; ROM: 5.0.0.102.

### Permission Requirements

None.

### API requirements

> [!TIP] All versions of the current third-party libraries have implemented version isolation, supporting compilation in `API12+` projects and execution on `API12+` ROMs.

## Example

The following code shows the basic use scenario of the repository:

> [!WARNING] The import name stays the same: import from `react-native-xlog` on the JS side (alias import for Autolink projects; `@react-native-ohos/react-native-xlog` for manually har-linked projects; both are equivalent).

```javascript
import XLog from 'react-native-xlog';

// Optional on the host side: register the configuration in PackageProvider.cpp first
// (without registration the defaults are used: logPath=app sandbox cache, level=All,
//  showConsoleLog=true, async appender, file name prefix "Test")
// RNXLogPackage::registerXLog("/data/storage/el2/base/haps/entry/cache/xlog", 0, true, 0, "Test");

await XLog.open();
XLog.info('tag', 'hello xlog');
XLog.error('tag', 'something failed');
await XLog.close();
```

## How to Use

**Lifecycle management**

```javascript
import XLog from 'react-native-xlog';

// Open xlog (reads the configuration registered by registerXLog; defaults are used when not registered)
await XLog.open();

// Close xlog (also uninstalls the crash handler and restores the default console behavior)
await XLog.close();
```

**Six log levels**

```javascript
XLog.verbose('tag', '...');
XLog.debug('tag', '...');
XLog.info('tag', '...');
XLog.warn('tag', '...');
XLog.error('tag', '...');
XLog.fatal('tag', '...');
```

**Crash capture**

```javascript
// Open xlog before installing the crash handler
await XLog.open();
// Install the crash handler
await XLog.installUncaughtCrashHandler();

// Uninstall (close() also uninstalls it)
await XLog.uninstallUncaughtCrashHandler();

// On HarmonyOS, restart the app and call open() to write the signal number to xlog
```

**console redirection**

```javascript
// After open, JS console.log/info/warn/error output is written to xlog
// (echoed in hilog in xlog format, tag=RN); after close it returns to the framework default (hilog)
await XLog.open();
console.log('this line goes to xlog');
await XLog.close();
console.log('this line goes back to the default hilog output');
```

## Available APIs

> [!TIP] The **Platform** column indicates the platform where the properties are supported in the original third-party library.

> [!TIP] If the value of **HarmonyOS Support** is **yes**, it means that the OpenHarmony platform supports this property; **no** means the opposite; **partially** means some capabilities of this property are supported. The usage method is the same on different platforms and the effect is the same as that of iOS or Android.

This library exposes the `XLog` module (the JS wrapper of TurboModule `XLogBridge`). It has no exported properties/constants; all APIs are methods:

### Methods

| Name | Description | Type | Platform | HarmonyOS Support |
| - | - | - | - | - |
| open | Open xlog (reads the host-registered configuration; defaults are used when not registered) and redirects JS console output to xlog | `() => Promise<void>` | iOS / Android | yes |
| close | Close xlog (uninstalls the crash handler and restores the default console output) | `() => Promise<void>` | iOS / Android | yes |
| verbose | Write one log entry at verbose level | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| debug | Write one log entry at debug level | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| info | Write one log entry at info level | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| warn | Write one log entry at warn level | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| error | Write one log entry at error level | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| fatal | Write one log entry at fatal level | `(tag: string, message: string) => Promise<void>` | iOS / Android | yes |
| installUncaughtCrashHandler | Install the crash handler; app crashes are written to xlog | `() => Promise<void>` | iOS / Android | yes |
| uninstallUncaughtCrashHandler | Uninstall the crash signal handler | `() => Promise<void>` | iOS / Android | yes |

## Known Issues

None

## Other

None

## Directory Structure

````text
/xlog                                    # project root
├── harmony                              # HarmonyOS adaptation code
│   ├── rn_xlog.har                      # har package
│   └── rn_xlog                          # HarmonyOS adaptation core code
│       ├── Index.ets                    # entry of the HarmonyOS adaptation code
│       ├── ts.ets                       # ArkTS-side type export entry
│       └── src/main
│           ├── ets
│           │   ├── RNXLogPackage.ets    # ArkTS Package (TurboModule registration hub)
│           │   ├── RNXLogModule.ets     # ArkTS TurboModule (thin wrapper, logic lives in cpp)
│           │   └── generated/           # ArkTS code generated by codegen
│           └── cpp
│               ├── CMakeLists.txt       # C++ build config (target rnoh_xlog)
│               ├── RNXLogPackage.h      # Package factory overriding the codegen base class
│               ├── RNXLogModule.cpp/.h  # TurboModule cpp implementation (open/close/log/crash)
│               ├── XLogCrashHandler.cpp/.h  # crash signal handler
│               ├── XLogNativeLogger.h   # cross-version declaration of bindNativeLogger
│               ├── mars/                # mars xlog headers
│               └── generated/           # cpp code generated by codegen
├── example                              # example project (upstream example + rnt manual test page)
│   ├── src/Home.js                      # home: upstream / rnt entries
│   ├── src/UpstreamExample.js           # upstream example page
│   ├── src/XLogTest.tsx                 # rnt: manual test page (per-API buttons + call log)
│   ├── src/types/                       # TS type shims
│   └── harmony                          # DevEco project
├── buildEnv.sh                          # build environment variables
├── index.js                             # JS entry (10 exported APIs)
├── README.md                            # Chinese documentation
└── README_en.md                         # English documentation
````

## How to Contribute

Issues are welcome at [Issue](https://github.com/react-native-oh-library/react-native-xlog/issues), and [PRs](https://github.com/react-native-oh-library/react-native-xlog/pulls) are very welcome too.

## License

This project is based on the [MIT License](https://github.com/EngsShi/react-native-xlog/blob/master/LICENSE). Enjoy and contribute to open source freely.

