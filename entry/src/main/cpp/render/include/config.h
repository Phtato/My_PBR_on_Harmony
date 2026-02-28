#ifndef VKNDKEXAMPLE_CONFIG_H
#define VKNDKEXAMPLE_CONFIG_H

#include <vulkan/vulkan.h>

struct Settings
{
    // todo 鸿蒙现在支持校验层了吗？
    bool enableValidation;
    VkExtent2D extent;
    bool vsync;
    bool multiSampling;
    VkSampleCountFlagBits sampleCount;
};

inline constexpr Settings DEFAULT_SETTINGS{
    .enableValidation = false,
    .extent = {1260, 2506},
    .vsync = true,
    .multiSampling = true,
    .sampleCount = VK_SAMPLE_COUNT_4_BIT
};

#endif //VKNDKEXAMPLE_CONFIG_H