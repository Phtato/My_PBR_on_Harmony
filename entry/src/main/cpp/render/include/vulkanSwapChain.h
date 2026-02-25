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

    VulkanSwapChain() = default;
    VulkanSwapChain(const VulkanSwapChain&) = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;
    VulkanSwapChain(VulkanSwapChain&&) = delete;
    VulkanSwapChain& operator=(VulkanSwapChain&&) = delete;

private:
    VkInstance m_instance;
    Settings m_settings;
    VkFormat m_color_format;
    VkColorSpaceKHR m_color_space;
    uint32_t imageCount;
    std::vector<SwapChainBuffer> buffers;
    std::vector<VkImage> images;
    uint32_t queueNodeIndex = UINT32_MAX;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkDevice m_device;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    void initSurface();
};


#endif //VKNDKEXAMPLE_VULKANSWAPCHAIN_H