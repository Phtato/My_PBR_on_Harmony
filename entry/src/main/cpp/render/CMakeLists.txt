add_library(render STATIC
    main.cpp
        include/vulkan_base.h
        src/vulkan_base.cpp
        include/common.h
        include/config.h
)

# Enable C++20 module support for render library
target_compile_options(render PRIVATE -fmodules -fmodules-cache-path=${CMAKE_BINARY_DIR}/pcm)
target_compile_features(render PUBLIC cxx_std_20)
