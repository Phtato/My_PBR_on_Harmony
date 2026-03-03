//
// Created by 新垣つつ on 2026/3/2.
//
// NOTE: 此类设计未完成，暂不使用。考虑参数过多且接近原生 API，RAII 封装的价值有限。

#ifndef VKNDKEXAMPLE_VULKAN_IMAGE_H
#define VKNDKEXAMPLE_VULKAN_IMAGE_H
#include <vulkan/vulkan_core.h>

#include "vulkan_device.h"

/**
 * @brief RAII 风格的 Vulkan Image 封装
 *
 * 自动管理 VkImage、VkImageView、VkDeviceMemory 和 VkSampler 的生命周期
 */
class VulkanImage {
public:
    /**
     * @brief 构造函数：创建 image 并分配内存
     *
     * @param device Vulkan 设备对象引用
     * @param width 图像宽度（像素）
     * @param height 图像高度（像素）
     * @param format 图像格式（如 VK_FORMAT_R8G8B8A8_SRGB）
     * @param usage 图像用途标志位
     *              - VK_IMAGE_USAGE_TRANSFER_DST_BIT: 传输目标
     *              - VK_IMAGE_USAGE_SAMPLED_BIT: 着色器采样
     *              - VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT: 颜色附件
     *              - VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT: 深度模板附件
     * @param aspect 图像视图的方面标志
     *               - VK_IMAGE_ASPECT_COLOR_BIT: 颜色方面
     *               - VK_IMAGE_ASPECT_DEPTH_BIT: 深度方面
     */
    VulkanImage(const VulkanDevice& device,
                uint32_t width,
                uint32_t height,
                VkFormat format,
                VkImageUsageFlags usage,
                VkImageAspectFlags aspect);

    /**
     * @brief 析构函数：自动清理所有资源
     */
    ~VulkanImage();

    // 禁用拷贝
    VulkanImage(const VulkanImage&) = delete;
    VulkanImage& operator=(const VulkanImage&) = delete;

    // 允许移动
    VulkanImage(VulkanImage&& other) noexcept;
    VulkanImage& operator=(VulkanImage&& other) noexcept;

    /**
     * @brief 获取底层 VkImage 句柄
     */
    VkImage get() const { return image_; }
    operator VkImage() const { return image_; }

    /**
     * @brief 获取 ImageView
     */
    VkImageView view() const { return view_; }

    /**
     * @brief 获取 Sampler（如果已创建）
     */
    VkSampler sampler() const { return sampler_; }

    /**
     * @brief 转换图像布局
     *
     * @param cmd 命令缓冲区
     * @param oldLayout 旧布局
     * @param newLayout 新布局
     */
    void TransitionLayout(VkCommandBuffer cmd,
                         VkImageLayout oldLayout,
                         VkImageLayout newLayout);

private:
    VulkanDevice   device_;
    VkImage        image_   = VK_NULL_HANDLE;
    VkImageView    view_    = VK_NULL_HANDLE;
    VkDeviceMemory memory_  = VK_NULL_HANDLE;
    VkSampler      sampler_ = VK_NULL_HANDLE;

    void Cleanup();
};


#endif //VKNDKEXAMPLE_VULKAN_IMAGE_H