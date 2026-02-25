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
    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    std::vector<VkQueueFamilyProperties> queue_families_;
    VkDevice logical_device_;
    // todo 考虑吧swapchain相关的拆出来
    VkQueueFamilyProperties queue_family_;       // 选择的队列族索引

    VkFormat color_format_;
    VkColorSpaceKHR color_space_;

    VkResult CreateInstance();

    VkResult SelectPhysicalDevice();

    uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags) const;

    VkResult CreateLogicalDevice();

    VkResult CreateSurface();

    void InitSurface();

    VkResult CreateSwapChain();
public:
    VulkanBase(const VulkanBase&) = delete;
    VulkanBase& operator=(const VulkanBase&) = delete;
    VulkanBase(VulkanBase&&) = delete;
    VulkanBase& operator=(VulkanBase&&) = delete;

    Settings settings_;
    std::unique_ptr<VulkanSwapChain> swap_chain_;

    VkResult InitVulkan(uint32_t width, uint32_t height);
};

#endif //VKNDKEXAMPLE_RENDER_CORE_H
