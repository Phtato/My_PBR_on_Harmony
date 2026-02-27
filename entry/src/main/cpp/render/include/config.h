//
// Created by bilibili on 2026/2/11.
//

#ifndef VKNDKEXAMPLE_CONFIG_H
#define VKNDKEXAMPLE_CONFIG_H

struct Settings
{
    // todo 鸿蒙现在支持校验层了吗？
    bool enableValida;
    VkExtent2D extent = {1260,2506};
    bool vsync;
    bool multiSampling = true;

    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_4_BIT;
};

#endif //VKNDKEXAMPLE_CONFIG_H