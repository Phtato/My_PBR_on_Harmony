add_library(render STATIC
    main.cpp
        include/vulkanBase/vulkan_base.h
        src/vulkanBase/vulkan_base.cpp
        include/common.h
        include/config.h
        src/vulkanBase/vulkan_swap_chain.cpp
        include/vulkanBase/vulkan_swap_chain.h
        src/vulkanBase/vulkan_device.cpp
        include/vulkanBase/vulkan_device.h
        src/application.cpp
        include/application.h
        src/Scene/Scene.cpp
        include/Scene/Scene.h
        src/Scene/Camera.cpp
        include/Scene/Camera.h
        include/Scene/UniformBuffer.h
        include/Scene/Renderable.h
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
