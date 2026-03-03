//
// Created by 新垣つつ on 2026/2/28.
//

#ifndef VKNDKEXAMPLE_VULKANBUFFER_H
#define VKNDKEXAMPLE_VULKANBUFFER_H
#include <vulkan/vulkan_core.h>
#include "vulkan_device.h"


class VulkanBuffer
{
public:
    /**
     * @brief 构造函数：创建 buffer 并分配内存
     *
     * @param device Vulkan 设备对象引用，提供逻辑设备和内存查询功能
     * @param size Buffer 的字节大小
     * @param usage Buffer 用途标志位，指定如何使用此 buffer
     *              - VK_BUFFER_USAGE_VERTEX_BUFFER_BIT: 顶点缓冲
     *              - VK_BUFFER_USAGE_INDEX_BUFFER_BIT: 索引缓冲
     *              - VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT: Uniform 缓冲
     *              - VK_BUFFER_USAGE_STORAGE_BUFFER_BIT: 存储缓冲
     *              - VK_BUFFER_USAGE_TRANSFER_SRC_BIT: 传输源
     *              - VK_BUFFER_USAGE_TRANSFER_DST_BIT: 传输目标
     *              可以使用按位或 (|) 组合多个标志
     * @param props 内存属性标志位，指定内存的特性
     *              - VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT: 设备本地内存（GPU 专用，性能最佳）
     *              - VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT: CPU 可见内存（可映射）
     *              - VK_MEMORY_PROPERTY_HOST_COHERENT_BIT: 主机一致性内存（无需手动刷新）
     *              - VK_MEMORY_PROPERTY_HOST_CACHED_BIT: 主机缓存内存（读取优化）
     *              可以使用按位或 (|) 组合多个标志
     * @param sharingMode 共享模式，默认为 VK_SHARING_MODE_EXCLUSIVE（独占模式）
     *                    当前实现仅支持独占模式
     */
    VulkanBuffer(const VulkanDevice& device,
                  VkDeviceSize size,
                  VkBufferUsageFlags usage,
                  VkMemoryPropertyFlags props,
                  VkSharingMode sharingMode = VK_SHARING_MODE_EXCLUSIVE);

    // 析构函数：自动清理资源
    ~VulkanBuffer();

    // 禁用拷贝
    VulkanBuffer(const VulkanBuffer&) = delete;
    VulkanBuffer& operator=(const VulkanBuffer&) = delete;

    // 允许移动
    VulkanBuffer(VulkanBuffer&& other) noexcept;
    VulkanBuffer& operator=(VulkanBuffer&& other) noexcept;

    // 获取底层 VkBuffer（类似 vk::raii 的 operator*）
    VkBuffer get() const { return buffer_; }
    operator VkBuffer() const { return buffer_; }

    /**
     * @brief 内存操作方法
     *
     * 典型用法：
     * ```cpp
     * // 更新 Uniform Buffer
     * buffer.Map();
     * buffer.CopyTo(&mvpMatrix, sizeof(mvpMatrix));
     * buffer.Flush();  // 如果没有 COHERENT 标志则需要
     * buffer.Unmap();
     * ```
     */

    /**
     * @brief 将 GPU 内存映射到 CPU 地址空间
     *
     * 调用后可通过 mapped() 获取 CPU 侧指针，直接读写 GPU 内存。
     *
     * @note 仅对带 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 的内存有效
     * @note 可以在构造后一直保持映射状态（常用于 Uniform Buffer）
     */
    void Map();

    /**
     * @brief 取消内存映射
     *
     * 释放 CPU 地址空间，之后 mapped() 返回 nullptr。
     *
     * @note 对于频繁更新的 buffer，可以不调用 Unmap 保持持久映射
     */
    void Unmap();

    /**
     * @brief 将数据从 CPU 拷贝到 GPU 内存
     *
     * @param data 源数据指针
     * @param copySize 拷贝字节数
     *
     * @note 如果未映射会自动调用 Map()
     * @note 使用 memcpy 直接拷贝，需确保内存已映射
     */
    void CopyTo(const void* data, VkDeviceSize copySize);

    /**
     * @brief 刷新映射内存，确保 CPU 写入对 GPU 可见
     *
     * @note 仅在没有 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 时需要调用
     * @note COHERENT 内存会自动同步，无需手动 Flush
     */
    void Flush();

    // 访问器
    VkDeviceSize size() const { return size_; }
    void* mapped() const { return mapped_; }

private:
    VulkanDevice       device_;
    VkDeviceSize       size_;
    VkBuffer           buffer_ = VK_NULL_HANDLE;
    VkDeviceMemory     memory_ = VK_NULL_HANDLE;
    void*              mapped_ = nullptr;

    void Cleanup();
};


#endif //VKNDKEXAMPLE_VULKANBUFFER_H