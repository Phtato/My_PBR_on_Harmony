#ifndef VKNDKEXAMPLE_RENDER_CORE_H
#define VKNDKEXAMPLE_RENDER_CORE_H

#include <array>
#include <optional>
#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

#include "../config.h"
#include "vulkan_swap_chain.h"
#include "vulkan_device.h"
#include "vulkan_descriptor.h"

class VulkanBase
{
private:
    VkInstance instance_ = VK_NULL_HANDLE;

    struct FrameData {
        VkCommandBuffer cmd;
        VkSemaphore     imageAvailableSemaphore;
        VkSemaphore     renderFinishedSemaphore;
        VkFence         inFlightFence;
    };
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> frames_;

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

    VkResult prepare();

    void setupFrameBuffer();
public:
    VkResult InitVulkan(Settings settings);

    /**
     * @brief 获取全局 Descriptor Pool
     * 
     * Material 从这里分配 DescriptorSet
     */
    VulkanDescriptorPool& GetDescriptorPool() { return *descriptor_pool_; }

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
    std::unique_ptr<VulkanDescriptorPool> descriptor_pool_;  ///< 全局资源池
    DepthStencil depthStencil_;         // 全局深度
};

#endif //VKNDKEXAMPLE_RENDER_CORE_H
