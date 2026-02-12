//
// Created by bilibili on 2026/2/11.
//

#ifndef VKNDKEXAMPLE_COMMON_H
#define VKNDKEXAMPLE_COMMON_H
#include <stdexcept>
#include <sstream>
#include <vulkan/vulkan_core.h>

#define VK_CHECK(result) \
    do {    \
        if((result) != VK_SUCCESS) { \
            std::ostringstream oss;    \
            oss << "something error in " << __FUNCTION__  << " at " << __FILE__ <<  ":" << __LINE__; \
            throw std::runtime_error(oss.str());    \
        } \
    } while (0)\

#endif //VKNDKEXAMPLE_COMMON_H