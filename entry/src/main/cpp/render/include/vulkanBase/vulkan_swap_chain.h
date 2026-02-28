#ifndef VKNDKEXAMPLE_VULKAN_SWAP_CHAIN_H
#define VKNDKEXAMPLE_VULKAN_SWAP_CHAIN_H
#include <vector>
#include <vulkan/vulkan_core.h>

#include "../config.h"
#include "vulkan_device.h"

typedef struct _SwapChainBuffers {
    VkImage image;
    VkImageView view;
} SwapChainBuffer;

class VulkanSwapChain
{
public:
    VulkanSwapChain(VkInstance instance, Settings settings, VulkanDevice &device);
    VkResult CreateSurface();
    VkResult CreateSwapChain(uint32_t *width, uint32_t *height);
    void InitSurface();
    uint32_t queue_node_index_ = UINT32_MAX;
    VkFormat color_format_;
    VkColorSpaceKHR color_space_;
    uint32_t image_count_;
    std::vector<SwapChainBuffer> buffers_;

    VulkanSwapChain(const VulkanSwapChain&) = delete;
    VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;
    VulkanSwapChain(VulkanSwapChain&&) = delete;
    VulkanSwapChain& operator=(VulkanSwapChain&&) = delete;

private:
    VkInstance instance_;
    Settings settings_;

    std::vector<VkImage> images_;
    // VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    // VkDevice device_;
    VulkanDevice device_;
    VkSurfaceKHR surface_ = VK_NULL_HANDLE;
    VkSwapchainKHR swap_chain_ = VK_NULL_HANDLE;
};


#endif //VKNDKEXAMPLE_VULKAN_SWAP_CHAIN_H