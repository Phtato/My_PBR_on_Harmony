//
// Created by bilibili on 2026/2/24.
//

#ifndef VKNDKEXAMPLE_VULKANSWAPCHAIN_H
#define VKNDKEXAMPLE_VULKANSWAPCHAIN_H
#include <vector>
#include <vulkan/vulkan_core.h>

#include "config.h"

typedef struct _SwapChainBuffers {
    VkImage image;
    VkImageView view;
} SwapChainBuffer;

class VulkanSwapChain
{
public:
    VulkanSwapChain(VkInstance instance, Settings settings, VkPhysicalDevice physicalDevice, VkDevice m_device);
    VkResult CreateSurface();
    VkResult CreateSwapChain(uint32_t *width, uint32_t *height);
    void InitSurface();

    VulkanSwapChain() = default;
    VulkanSwapChain(const VulkanSwapChain&) = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;
    VulkanSwapChain(VulkanSwapChain&&) = delete;
    VulkanSwapChain& operator=(VulkanSwapChain&&) = delete;

private:
    VkInstance instance_;
    Settings settings_;
    VkFormat color_format_;
    VkColorSpaceKHR color_space_;
    uint32_t image_count_;
    std::vector<SwapChainBuffer> buffers_;
    std::vector<VkImage> images_;
    uint32_t queue_node_index_ = UINT32_MAX;
    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VkDevice device_;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;
};


#endif //VKNDKEXAMPLE_VULKANSWAPCHAIN_H