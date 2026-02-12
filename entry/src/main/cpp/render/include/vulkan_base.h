//
// Created by bilibili on 2026/2/10.
//

#ifndef VKNDKEXAMPLE_RENDER_CORE_H
#define VKNDKEXAMPLE_RENDER_CORE_H

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

#include "config.h"

class VulkanBase
{
private:
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    std::vector<VkQueueFamilyProperties> queueFamilies;
    VkDevice logicalDevice_;
    VkQueueFamilyProperties queueFamily_;       // 选择的队列族索引

    VkResult CreateInstance();

    VkResult SelectPhysicalDevice();

    uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags) const;

    VkResult CreateLogicalDevice();


public:
    VulkanBase(const VulkanBase&) = delete;
    VulkanBase& operator=(const VulkanBase&) = delete;
    VulkanBase(VulkanBase&&) = delete;
    VulkanBase& operator=(VulkanBase&&) = delete;

    Settings settings;
    VkResult InitVulkan();
};

#endif //VKNDKEXAMPLE_RENDER_CORE_H
