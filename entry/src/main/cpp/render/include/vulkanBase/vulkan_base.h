#ifndef VKNDKEXAMPLE_RENDER_CORE_H
#define VKNDKEXAMPLE_RENDER_CORE_H

#include <optional>
#include <vector>
#include <vulkan/vulkan.h>

#include "../config.h"
#include "vulkan_swap_chain.h"
#include "vulkan_device.h"

class VulkanBase
{
private:
    VkInstance instance_ = VK_NULL_HANDLE;


    struct MultisampleTarget {
        struct {
            VkImage image;
            VkImageView view;
            VkDeviceMemory memory;
        } color;
        struct {
            VkImage image;
            VkImageView view;
            VkDeviceMemory memory;
        } depth;
    } multisampleTarget;

    struct DepthStencil {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depthStencil;

    VkResult CreateInstance();

    VkResult GetDeviceCaps();

    void setupFrameBuffer();
public:
    VkResult InitVulkan(Settings settings);
    VkResult prepare();

protected:
    VkFormat color_format_;
    VkColorSpaceKHR color_space_;
    VkCommandPool cmd_pool_;
    VkFormat depth_format_;
    VkQueue queue_;
    VkRenderPass render_pass_;
    VkPipelineCache pipeline_cache_;
    std::vector<VkFramebuffer> frame_buffers_;

    Settings settings_;
    std::unique_ptr<VulkanDevice> device_;
    std::unique_ptr<VulkanSwapChain> swap_chain_;
};

#endif //VKNDKEXAMPLE_RENDER_CORE_H
