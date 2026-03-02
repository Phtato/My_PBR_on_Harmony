file(GLOB_RECURSE RENDER_SOURCES
    "src/**/*.cpp"
    "src/**/*.cc"
)
file(GLOB_RECURSE RENDER_HEADERS
    "include/**/*.h"
    "include/**/*.hpp"
)

add_library(render STATIC
    main.cpp
    ${RENDER_SOURCES}
    ${RENDER_HEADERS}
        src/vulkanBase/vulkan_image.cpp
        include/vulkanBase/vulkan_image.h
)

# Enable C++20 module support for render library
target_compile_options(render PRIVATE -fmodules -fmodules-cache-path=${CMAKE_BINARY_DIR}/pcm)
target_compile_features(render PUBLIC cxx_std_20)

# Fetch dependencies from GitHub
include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

# Vulkan-Hpp: 官方 C++ 绑定，纯头文件，版本与鸿蒙 SDK 的 vulkan_core.h (VK_HEADER_VERSION 309) 对应
FetchContent_Declare(
    VulkanHpp
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Hpp.git
    GIT_TAG        v1.4.309
)

FetchContent_MakeAvailable(glm VulkanHpp)

# Include directories
target_include_directories(render PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty
    ${vulkanhpp_SOURCE_DIR}/include  # 提供 <vulkan/vulkan.hpp>
)

# Link Vulkan library and glm
target_link_libraries(render PUBLIC vulkan glm::glm)
