//
// Created by 新垣つつ on 2026/3/6.
//

#ifndef VKNDKEXAMPLE_VULKAN_PIPELINE_LAYOUT_H
#define VKNDKEXAMPLE_VULKAN_PIPELINE_LAYOUT_H

#include <vector>
#include <vulkan/vulkan_core.h>

// ============================================================================
//  VulkanPipelineLayout
//  管理 VkPipelineLayout，组织多个 DescriptorSetLayout 和 Push Constants。
//  
//  典型用法：
//    VulkanPipelineLayout pipelineLayout(device, {
//        materialLayout.get(),  // Descriptor Set 0
//        globalLayout.get(),    // Descriptor Set 1
//    });
//
//    VkPipelineLayout vkLayout = pipelineLayout.get();
// ============================================================================
class VulkanPipelineLayout {
public:
    /**
     * @brief 构造函数：创建 VkPipelineLayout
     *
     * @param device           逻辑设备
     * @param setLayouts       Descriptor Set Layout 数组
     * @param pushConstantSize （可选）Push Constant 的字节大小，0 表示不使用
     */
    VulkanPipelineLayout(VkDevice device,
                         const std::vector<VkDescriptorSetLayout>& setLayouts,
                         uint32_t pushConstantSize = 0);

    ~VulkanPipelineLayout();

    // 禁用拷贝
    VulkanPipelineLayout(const VulkanPipelineLayout&)            = delete;
    VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;

    // 允许移动
    VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept;
    VulkanPipelineLayout& operator=(VulkanPipelineLayout&& other) noexcept;

    /**
     * @brief 获取底层 VkPipelineLayout
     *
     * 用于创建 VkGraphicsPipeline 或绑定到 Command Buffer
     */
    VkPipelineLayout get() const { return layout_; }

private:
    VkDevice         device_ = VK_NULL_HANDLE;
    VkPipelineLayout layout_ = VK_NULL_HANDLE;

    void Cleanup();
};


#endif //VKNDKEXAMPLE_VULKAN_PIPELINE_LAYOUT_H
