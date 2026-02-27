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
)

# Enable C++20 module support for render library
target_compile_options(render PRIVATE -fmodules -fmodules-cache-path=${CMAKE_BINARY_DIR}/pcm)
target_compile_features(render PUBLIC cxx_std_20)

# Link Vulkan library
target_link_libraries(render PUBLIC vulkan)
