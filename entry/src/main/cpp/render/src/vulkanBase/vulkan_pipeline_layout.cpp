//
// Created by 新垣つつ on 2026/3/6.
//

#include "render/include/vulkanBase/vulkan_pipeline_layout.h"
#include <stdexcept>

#include "render/include/common.h"

// ============================================================================
//  VulkanPipelineLayout
// ============================================================================

VulkanPipelineLayout::VulkanPipelineLayout(VkDevice device,
                                           const std::vector<VkDescriptorSetLayout>& setLayouts,
                                           uint32_t pushConstantSize)
    : device_(device)
{
    // TODO
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    Cleanup();
}

VulkanPipelineLayout::VulkanPipelineLayout(VulkanPipelineLayout&& other) noexcept
    : device_(other.device_), layout_(other.layout_)
{
    other.device_ = VK_NULL_HANDLE;
    other.layout_ = VK_NULL_HANDLE;
}

VulkanPipelineLayout& VulkanPipelineLayout::operator=(VulkanPipelineLayout&& other) noexcept
{
    if (this != &other) {
        Cleanup();
        device_  = other.device_;
        layout_  = other.layout_;
        other.device_ = VK_NULL_HANDLE;
        other.layout_ = VK_NULL_HANDLE;
    }
    return *this;
}

void VulkanPipelineLayout::Cleanup()
{
    // TODO
}
