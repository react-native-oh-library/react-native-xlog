cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)

set(rnoh_xlog_generated_dir "${CMAKE_CURRENT_SOURCE_DIR}/generated")

# 收集 codegen 生成的源文件（含 XLogBridge.cpp，满足 BaseReactNativeXlogPackage 的链接需求）
file(GLOB_RECURSE rnoh_xlog_generated_SRC
    "${rnoh_xlog_generated_dir}/*.cpp"
)

set(rnoh_xlog_SRC ${rnoh_xlog_generated_SRC})

if(NOT rnoh_xlog_SRC)
    message(FATAL_ERROR "rnoh_xlog: no .cpp sources found under ${CMAKE_CURRENT_SOURCE_DIR}")
endif()

# 构建共享库
add_library(rnoh_xlog SHARED
    ${rnoh_xlog_SRC}
    RNXLogModule.cpp
    XLogCrashHandler.cpp
)

target_include_directories(rnoh_xlog PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${rnoh_xlog_generated_dir}
)

target_link_libraries(rnoh_xlog PUBLIC rnoh)

# 链接 libmarsxlog.so
# 搜索路径1：项目根目录的 .ohpm store
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.ohpm/@ohos+xlog@1.0.0/oh_modules/@ohos/xlog/libs/${OHOS_ARCH}")
    set(XLOG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../.ohpm/@ohos+xlog@1.0.0/oh_modules/@ohos/xlog/libs/${OHOS_ARCH})
    message("XLOG_PATH1: ${XLOG_PATH}")
# 搜索路径2：HAR 内部的 oh_modules
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules/@ohos/xlog/libs/${OHOS_ARCH}")
    set(XLOG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../../oh_modules/@ohos/xlog/libs/${OHOS_ARCH})
    message("XLOG_PATH2: ${XLOG_PATH}")
else()
    message("XLOG_PATH: empty")
endif()

file(REAL_PATH "${XLOG_PATH}" XLOG_PATH)

set(XLOG_SO "${XLOG_PATH}/libmarsxlog.so")

target_link_libraries(rnoh_xlog PUBLIC ${XLOG_SO})
