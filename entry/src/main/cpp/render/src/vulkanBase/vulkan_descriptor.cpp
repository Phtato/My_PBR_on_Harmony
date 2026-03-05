//
// Created by 新垣つつ on 2026/3/3.
//

#include "render/include/vulkanBase/vulkan_descriptor.h"
#include <stdexcept>

#include "render/include/common.h"

// ============================================================================
//  VulkanDescriptorPool
// ============================================================================

VulkanDescriptorPool::VulkanDescriptorPool(VkDevice device,
                                           uint32_t maxSets,
                                           const std::vector<VkDescriptorPoolSize>& sizes)
    : device_(device)
{
    VkDescriptorPoolSize poolSize{
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = static_cast<uint32_t>(sizes.size())
    };
    VkDescriptorPoolCreateInfo descPoolCI{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = maxSets,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize
    };
    VK_CHECK(vkCreateDescriptorPool(device, &descPoolCI, nullptr, &pool_));
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    Cleanup();
}

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDescriptorPool&& other) noexcept
    : device_(other.device_), pool_(other.pool_)
{
    other.device_ = VK_NULL_HANDLE;
    other.pool_   = VK_NULL_HANDLE;
}

VulkanDescriptorPool& VulkanDescriptorPool::operator=(VulkanDescriptorPool&& other) noexcept
{
    if (this != &other) {
        Cleanup();
        device_  = other.device_;
        pool_    = other.pool_;
        other.device_ = VK_NULL_HANDLE;
        other.pool_   = VK_NULL_HANDLE;
    }
    return *this;
}

VkDescriptorSet VulkanDescriptorPool::Allocate(VkDescriptorSetLayout layout)
{
    // TODO

/*
    typedef struct VkDescriptorSetAllocateInfo {
        VkStructureType                 sType;
        const void*                     pNext;
        VkDescriptorPool                descriptorPool;
        uint32_t                        descriptorSetCount;
        const VkDescriptorSetLayout*    pSetLayouts;
    } VkDescriptorSetAllocateInfo;
*/
/*
     VkResult vkAllocateDescriptorSets(
        VkDevice                                    device,
        const VkDescriptorSetAllocateInfo*          pAllocateInfo,
        VkDescriptorSet*                            pDescriptorSets);
*/
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool_;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet set;
    VK_CHECK(vkAllocateDescriptorSets(device_, &allocInfo, &set));

    return set;
}

void VulkanDescriptorPool::Reset()
{
    VkResult result = vkResetDescriptorPool(device_, pool_, 0);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("vkResetDescriptorPool failed!");
    }
}

void VulkanDescriptorPool::Cleanup()
{
    if (pool_ != VK_NULL_HANDLE && device_ != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device_, pool_, nullptr);
    }
    pool_ = VK_NULL_HANDLE;
    device_ = VK_NULL_HANDLE;
}


// ============================================================================
//  VulkanDescriptorLayout
// ============================================================================

VulkanDescriptorLayout::VulkanDescriptorLayout(VkDevice device,
                                               const std::vector<VkDescriptorSetLayoutBinding>& bindings)
    : device_(device)
{
    // TODO
    /*
    typedef struct VkDescriptorSetLayoutCreateInfo {
        VkStructureType                        sType;
        const void*                            pNext;
        VkDescriptorSetLayoutCreateFlags       flags;
        uint32_t                               bindingCount;
        const VkDescriptorSetLayoutBinding*    pBindings;
    } VkDescriptorSetLayoutCreateInfo;
    */

    /*
    VkResult vkCreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout);
    */

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(bindings.size()),
        .pBindings = bindings.data()
    };

    VK_CHECK(vkCreateDescriptorSetLayout(device_, &layoutInfo, nullptr,  &layout_));
}

VulkanDescriptorLayout::~VulkanDescriptorLayout()
{
    Cleanup();
}

VulkanDescriptorLayout::VulkanDescriptorLayout(VulkanDescriptorLayout&& other) noexcept
    : device_(other.device_), layout_(other.layout_)
{
    other.device_ = VK_NULL_HANDLE;
    other.layout_ = VK_NULL_HANDLE;
}

VulkanDescriptorLayout& VulkanDescriptorLayout::operator=(VulkanDescriptorLayout&& other) noexcept
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

void VulkanDescriptorLayout::UpdateBuffer(VkDescriptorSet  set,
                                          uint32_t         binding,
                                          VkBuffer         buffer,
                                          VkDeviceSize     offset,
                                          VkDeviceSize     size,
                                          VkDescriptorType type)
{
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = buffer;
    bufferInfo.offset = offset;
    bufferInfo.range = size;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = set;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = type;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device_, 1, &descriptorWrite, 0, nullptr);
}


void VulkanDescriptorLayout::UpdateImage(VkDescriptorSet set,
                                         uint32_t        binding,
                                         VkImageView     view,
                                         VkSampler       sampler,
                                         VkImageLayout   imgLayout)
{
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageView = view;
    imageInfo.sampler = sampler;
    imageInfo.imageLayout = imgLayout;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = set;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device_, 1, &descriptorWrite, 0, nullptr);

}

void VulkanDescriptorLayout::Cleanup()
{
    if (layout_ != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(device_, layout_, nullptr);
        layout_ = VK_NULL_HANDLE;
    }
}
