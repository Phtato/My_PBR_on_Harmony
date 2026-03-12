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
        src/ShaderLibrary.cpp
)

# 确保输出目录存在
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/rawfile/data)

# 定义 Shader 编译命令
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../resources/rawfile/data)

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
# NOTE: vulkan-hpp include paths are provided automatically via VulkanHpp CMake target
target_include_directories(render PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty
)

add_custom_command(
    TARGET render
    PRE_BUILD
    COMMENT "Compiling shaders..."
    COMMAND glslc -fshader-stage=vert ${CMAKE_CURRENT_SOURCE_DIR}/shader/shader.vert -o ${SHADER_OUTPUT_DIR}/vert.spv
    COMMAND glslc -fshader-stage=frag ${CMAKE_CURRENT_SOURCE_DIR}/shader/shader.frag -o ${SHADER_OUTPUT_DIR}/frag.spv
)

# Link Vulkan library, vulkan-hpp and glm
target_link_libraries(render PUBLIC VulkanHpp vulkan glm::glm)