#include "render/include/common.h"
#include "render/include/vulkanBase/vulkan_buffer.h"


vulkan_buffer::vulkan_buffer(const VulkanDevice &device,
                             VkDeviceSize size,
                             VkBufferUsageFlags usage,
                             VkMemoryPropertyFlags props,
                             VkSharingMode sharingMode)
    : device_(device),
      size_(size)
{
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size_;
    info.usage = usage;
    info.sharingMode = sharingMode;

    if (sharingMode != VK_SHARING_MODE_EXCLUSIVE) {
        throw std::runtime_error("only support VK_SHARING_MODE_EXCLUSIVE currently");
    }

    VK_CHECK(vkCreateBuffer(device_.logical_device_, &info, nullptr, &buffer_));

    VkMemoryRequirements memReqs;
    vkGetBufferMemoryRequirements(device_.logical_device_, buffer_, &memReqs);
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.allocationSize = memReqs.size;
    memAllocInfo.memoryTypeIndex = device_.FindMemoryType(memReqs.memoryTypeBits, props);
    VK_CHECK(vkAllocateMemory(device_.logical_device_, &memAllocInfo, nullptr, &memory_));
    VK_CHECK(vkBindBufferMemory(device_.logical_device_, buffer_, memory_, 0));
}

vulkan_buffer::~vulkan_buffer() {
    Cleanup();
}

void vulkan_buffer::Map() {
    VK_CHECK(vkMapMemory(
        device_.logical_device_,
        memory_,           // 要映射的GPU内存
        0,                 // 偏移量
        size_,             // 映射大小
        0,                 // 标志位（保留）
        &mapped_           // 输出：CPU侧的指针
    ));
}

void vulkan_buffer::Unmap() {
    if (mapped_) {
        vkUnmapMemory(device_.logical_device_, memory_);
        mapped_ = nullptr;
    }
}

void vulkan_buffer::CopyTo(const void* data, VkDeviceSize copySize) {
    if (!mapped_) Map();
    memcpy(mapped_, data, copySize);  // 直接用CPU拷贝
}

void vulkan_buffer::Flush() {
    VkMappedMemoryRange range{};
    range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    range.memory = memory_;
    range.offset = 0;
    range.size = size_;
    vkFlushMappedMemoryRanges(device_.logical_device_, 1, &range);
}

void vulkan_buffer::Cleanup() {
    // 如果内存仍在映射状态，先取消映射
    if (mapped_) {
        Unmap();
    }

    // 销毁 Buffer 对象
    if (buffer_ != VK_NULL_HANDLE) {
        vkDestroyBuffer(device_.logical_device_, buffer_, nullptr);
        buffer_ = VK_NULL_HANDLE;
    }

    // 释放设备内存
    if (memory_ != VK_NULL_HANDLE) {
        vkFreeMemory(device_.logical_device_, memory_, nullptr);
        memory_ = VK_NULL_HANDLE;
    }
}






