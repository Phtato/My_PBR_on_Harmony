//
// Created by 新垣つつ on 2026/3/3.
//

#ifndef VKNDKEXAMPLE_VULKAN_DESCRIPTOR_H
#define VKNDKEXAMPLE_VULKAN_DESCRIPTOR_H

#include <vector>
#include <vulkan/vulkan_core.h>

// ============================================================================
//  VulkanDescriptorPool
//  全局唯一的描述符内存池，生命周期与 Application 相同。
//  负责向驱动预申请资源槽位，所有 Layout 的 Set 都从这里分配。
//
//  典型用法：
//    VulkanDescriptorPool pool(device, 64, {
//        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         64},
//        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 128},
//    });
//    VkDescriptorSet set = pool.Allocate(layout);
// ============================================================================
class VulkanDescriptorPool {
public:
    /**
     * @brief 构造函数：创建 VkDescriptorPool
     *
     * @param device   逻辑设备
     * @param maxSets  池中最多可分配的 DescriptorSet 总数（所有类型之和）
     * @param sizes    每种描述符类型的容量声明
     *                 e.g. {{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 32}, ...}
     */
    VulkanDescriptorPool(VkDevice device,
                         uint32_t maxSets,
                         const std::vector<VkDescriptorPoolSize>& sizes);

    ~VulkanDescriptorPool();

    // 禁用拷贝
    VulkanDescriptorPool(const VulkanDescriptorPool&)            = delete;
    VulkanDescriptorPool& operator=(const VulkanDescriptorPool&) = delete;

    // 允许移动
    VulkanDescriptorPool(VulkanDescriptorPool&& other) noexcept;
    VulkanDescriptorPool& operator=(VulkanDescriptorPool&& other) noexcept;

    /**
     * @brief 从池中分配一个 DescriptorSet
     *
     * @param layout 用于分配的 VkDescriptorSetLayout
     * @return 分配成功的 VkDescriptorSet
     * @throws std::runtime_error 池已满或分配失败
     */
    VkDescriptorSet Allocate(VkDescriptorSetLayout layout);

    /**
     * @brief 重置池，一次性回收所有已分配的 Set（无需逐个释放）
     *
     * @note 适合每帧重建 Set 的场景（如动态 UI），普通 PBR 渲染不常用
     */
    void Reset();

    VkDescriptorPool get() const { return pool_; }

private:
    VkDevice         device_ = VK_NULL_HANDLE;
    VkDescriptorPool pool_   = VK_NULL_HANDLE;

    void Cleanup();
};


// ============================================================================
//  VulkanDescriptorLayout
//  管理一种 VkDescriptorSetLayout，并提供 UpdateBuffer / UpdateImage 辅助。
//  不持有 Pool，从外部的 VulkanDescriptorPool 分配 Set。
//
//  典型用法（Material 初始化时）：
//    VulkanDescriptorLayout matLayout(device, {
//        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
//        {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
//    });
//
//    VkDescriptorSet set = pool.Allocate(matLayout.get());
//    matLayout.UpdateBuffer(set, 0, ubo, 0, sizeof(UBO));
//    matLayout.UpdateImage (set, 1, texView, sampler);
// ============================================================================
class VulkanDescriptorLayout {
public:
    /**
     * @brief 构造函数：创建 VkDescriptorSetLayout
     *
     * @param device   逻辑设备
     * @param bindings 各 binding 的描述（类型、数量、着色器阶段）
     */
    VulkanDescriptorLayout(VkDevice device,
                           const std::vector<VkDescriptorSetLayoutBinding>& bindings);

    ~VulkanDescriptorLayout();

    // 禁用拷贝
    VulkanDescriptorLayout(const VulkanDescriptorLayout&)            = delete;
    VulkanDescriptorLayout& operator=(const VulkanDescriptorLayout&) = delete;

    // 允许移动
    VulkanDescriptorLayout(VulkanDescriptorLayout&& other) noexcept;
    VulkanDescriptorLayout& operator=(VulkanDescriptorLayout&& other) noexcept;

    /**
     * @brief 获取底层 VkDescriptorSetLayout
     *
     * 用于：
     *  - pool.Allocate(layout.get())
     *  - 创建 VkPipelineLayout 时传入
     */
    VkDescriptorSetLayout get() const { return layout_; }

    /**
     * @brief 将 Buffer 写入 DescriptorSet 的指定 binding（便捷版）
     *
     * 适用类型：VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER / STORAGE_BUFFER
     *
     * @param set      目标 DescriptorSet
     * @param binding  目标 binding 编号
     * @param buffer   VkBuffer 句柄
     * @param offset   Buffer 内起始偏移（字节），通常为 0
     * @param size     有效数据字节数，VK_WHOLE_SIZE 表示整个 buffer
     * @param type     描述符类型，默认 UNIFORM_BUFFER
     */
    void UpdateBuffer(VkDescriptorSet  set,
                      uint32_t         binding,
                      VkBuffer         buffer,
                      VkDeviceSize     offset,
                      VkDeviceSize     size,
                      VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    /**
     * @brief 将 Buffer 写入 DescriptorSet 的指定 binding（底层版）
     *
     * 直接传入 VkDescriptorBufferInfo，用于高级场景
     *
     * @param set      目标 DescriptorSet
     * @param binding  目标 binding 编号
     * @param bufferInfo  Buffer 完整信息
     * @param type     描述符类型，默认 UNIFORM_BUFFER
     */
    void UpdateBufferDirect(VkDescriptorSet  set,
                            uint32_t         binding,
                            const VkDescriptorBufferInfo& bufferInfo,
                            VkDescriptorType type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    /**
     * @brief 将 Image + Sampler 写入 DescriptorSet 的指定 binding
     *
     * 适用类型：VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
     *
     * @param set       目标 DescriptorSet
     * @param binding   目标 binding 编号
     * @param view      图像的 VkImageView
     * @param sampler   采样器句柄
     * @param imgLayout 图像当前布局，通常为 SHADER_READ_ONLY_OPTIMAL
     */
    void UpdateImage(VkDescriptorSet set,
                     uint32_t        binding,
                     VkImageView     view,
                     VkSampler       sampler,
                     VkImageLayout   imgLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

private:
    VkDevice              device_ = VK_NULL_HANDLE;
    VkDescriptorSetLayout layout_ = VK_NULL_HANDLE;

    void Cleanup();
};


#endif //VKNDKEXAMPLE_VULKAN_DESCRIPTOR_H
