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

# so 已 vendored 进库(来源:openharmony_tpc_samples/XLog 本地构建,arm64-v8a),
# 随 har/hap 自动打包,不再依赖 ohpm 包 @ohos/xlog
set(XLOG_SO "${CMAKE_CURRENT_SOURCE_DIR}/../../../libs/${OHOS_ARCH}/libmarsxlog.so")
if(NOT EXISTS "${XLOG_SO}")
    message(FATAL_ERROR "rnoh_xlog: missing vendored ${XLOG_SO} (only arm64-v8a is shipped)")
endif()

target_link_libraries(rnoh_xlog PUBLIC ${XLOG_SO})
