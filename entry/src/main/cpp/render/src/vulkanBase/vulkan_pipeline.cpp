//
// Created by 新垣つつ on 2026/3/5.
//

#include "render/include/vulkanBase/vulkan_pipeline.h"
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
    VkPushConstantRange pushConstantRange{
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = pushConstantSize
    };

    VkPipelineLayoutCreateInfo layoutCI{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = static_cast<uint32_t>(setLayouts.size()),
        .pSetLayouts = setLayouts.data(),
        .pushConstantRangeCount = 1,
        .pPushConstantRanges = &pushConstantRange
    };

    VK_CHECK(vkCreatePipelineLayout(device, &layoutCI, nullptr, &layout_));
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
    if (layout_ != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device_, layout_, nullptr);
        layout_ = VK_NULL_HANDLE;
    }
}


// ============================================================================
//  VulkanGraphicsPipeline
// ============================================================================

VulkanGraphicsPipeline::VulkanGraphicsPipeline(VkDevice device, const CreateInfo& info)
    : device_(device)
{

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
