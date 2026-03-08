//
// Created by 新垣つつ on 2026/3/6.
//

#include "render/include/vulkanBase/vulkan_graphics_pipeline.h"
#include <stdexcept>

#include "render/include/common.h"

// ============================================================================
//  VulkanGraphicsPipeline
// ============================================================================

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, const CreateInfo& info)
    : device_(device)
{
    // TODO

}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
    Cleanup();
}

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanGraphicsPipeline&& other) noexcept
    : device_(other.device_), pipeline_(other.pipeline_)
{
    other.device_ = VK_NULL_HANDLE;
    other.pipeline_ = VK_NULL_HANDLE;
}

VulkanGraphicsPipeline& VulkanGraphicsPipeline::operator=(VulkanGraphicsPipeline&& other) noexcept
{
    if (this != &other) {
        Cleanup();
        device_    = other.device_;
        pipeline_  = other.pipeline_;
        other.device_ = VK_NULL_HANDLE;
        other.pipeline_ = VK_NULL_HANDLE;
    }
    return *this;
}

void VulkanGraphicsPipeline::Cleanup()
{
    // TODO
}
