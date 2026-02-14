# the minimum version of CMake.
cmake_minimum_required(VERSION 3.25.0)

# Default to the OpenHarmony (OHOS) toolchain when not provided from the command line or IDE profile.
# Note: Changing the toolchain requires regenerating the build directory/cache.
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "/Applications/DevEco-Studio.app/Contents/sdk/default/openharmony/native/build/cmake/ohos.toolchain.cmake" CACHE FILEPATH "OHOS toolchain file")
endif()
message(STATUS "Using CMAKE_TOOLCHAIN_FILE='${CMAKE_TOOLCHAIN_FILE}'")

project(vkNDKExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable C++20 module support
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "5beafd94-d8a5-4038-9143-e992b3718dda")
add_compile_options(-fmodules -fmodules-cache-path=${CMAKE_BINARY_DIR}/pcm)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED PACKAGE_FIND_FILE)
    include(${PACKAGE_FIND_FILE})
endif()

add_definitions(-DVK_USE_PLATFORM_OHOS)

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(entry SHARED napi_init.cpp
        VulkanConfig.cpp
        include/nativeStreamBuf.h)
target_link_libraries(entry
    PUBLIC libace_napi.z.so
    PUBLIC libace_ndk.z.so
    PUBLIC libnative_window.so  # 窗口系统相关
    PUBLIC rawfile.z            # 资源文件库
    PUBLIC libhilog_ndk.z.so
)

add_subdirectory(render)

target_link_libraries(entry PUBLIC render)
target_compile_features(entry PUBLIC cxx_std_20)