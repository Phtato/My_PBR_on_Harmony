//
// Created by 新垣つつ on 2026/3/2.
//

#include "../../include/vulkanBase/vulkan_image.h"


VulkanImage::VulkanImage(const VulkanDevice &device, uint32_t width, uint32_t height, VkFormat format,
                         VkImageUsageFlags usage, VkImageAspectFlags aspect) :
    device_(device)
{
/*
    *VkResult vkCreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage);
*/

/*
    typedef struct VkImageCreateInfo {
        VkStructureType          sType;
        const void*              pNext;
        VkImageCreateFlags       flags;
        VkImageType              imageType;
        VkFormat                 format;
        VkExtent3D               extent;
        uint32_t                 mipLevels;
        uint32_t                 arrayLayers;
        VkSampleCountFlagBits    samples;
        VkImageTiling            tiling;
        VkImageUsageFlags        usage;
        VkSharingMode            sharingMode;
        uint32_t                 queueFamilyIndexCount;
        const uint32_t*          pQueueFamilyIndices;
        VkImageLayout            initialLayout;
    } VkImageCreateInfo;
*/
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;


}
