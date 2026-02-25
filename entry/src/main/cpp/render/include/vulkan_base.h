//
// Created by bilibili on 2026/2/10.
//

#ifndef VKNDKEXAMPLE_RENDER_CORE_H
#define VKNDKEXAMPLE_RENDER_CORE_H

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

#include "config.h"
#include "vulkanSwapChain.h"

class VulkanBase
{
private:
    VkInstance instance_ = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
    std::vector<VkQueueFamilyProperties> queueFamilies;
    VkDevice logicalDevice_;
    // todo 考虑吧swapchain相关的拆出来
    VkQueueFamilyProperties queueFamily_;       // 选择的队列族索引

    VkFormat colorFormat;
    VkColorSpaceKHR colorSpace;

    VkResult CreateInstance();

    VkResult SelectPhysicalDevice();

    uint32_t getQueueFamilyIndex(VkQueueFlagBits queueFlags) const;

    VkResult CreateLogicalDevice();

    VkResult CreateSurface();

    void initSurface();

    VkResult CreateSwapChain();
public:
    VulkanBase(const VulkanBase&) = delete;
    VulkanBase& operator=(const VulkanBase&) = delete;
    VulkanBase(VulkanBase&&) = delete;
    VulkanBase& operator=(VulkanBase&&) = delete;

    Settings settings;
    std::unique_ptr<VulkanSwapChain> swapChain;

    VkResult InitVulkan(uint32_t width, uint32_t height);
};

#endif //VKNDKEXAMPLE_RENDER_CORE_H
