//
// Created by bilibili on 2026/2/24.
//

#include "render/include/vulkanBase/vulkan_swap_chain.h"

#include <vulkan/vulkan_ohos.h>

#include "VulkanConfig.h"
#include "render/include/common.h"

VulkanSwapChain::VulkanSwapChain(VkInstance instance, Settings settings, VulkanDevice &device)
    : instance_(instance),
      settings_(settings),
      device_(device)
{
    CreateSurface();
	InitSurface();
	// todo 这个写法是不对的，暂时懒得改
	CreateSwapChain(&settings_.extent.width, &settings_.extent.height);
}

VkResult VulkanSwapChain::CreateSurface()
{
    const VkSurfaceCreateInfoOHOS create_info{
        .sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS,
        .pNext = nullptr,
        .flags = 0,
        .window = VulkanConfig::getInstance().getWindow()
    };

    return vkCreateSurfaceOHOS(instance_, &create_info, nullptr, &surface_);
}

void VulkanSwapChain::InitSurface()
{
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device_.physical_device_, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device_.physical_device_, &queueCount, queueProps.data());

    std::vector<VkBool32> supportsPresent(queueCount);
    for (uint32_t i = 0; i < queueCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(device_.physical_device_, i, surface_, &supportsPresent[i]);
    }

    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;

    for (uint32_t i = 0; i < queueCount; i++)
    {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (graphicsQueueNodeIndex == UINT32_MAX)
            {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE)
            {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    if (presentQueueNodeIndex == UINT32_MAX)
    {
        for (uint32_t i = 0; i < queueCount; ++i)
        {
            if (supportsPresent[i] == VK_TRUE)
            {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
        throw std::runtime_error("Could not find a graphics and/or presenting queue!");

    if (graphicsQueueNodeIndex != presentQueueNodeIndex)
        throw std::runtime_error("Separate graphics and presenting queues are not supported yet!");

    queue_node_index_ = graphicsQueueNodeIndex;

    // Get a list of supported surface formats
    uint32_t formatCount;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device_.physical_device_, surface_, &formatCount, NULL));

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device_.physical_device_, surface_, &formatCount, surfaceFormats.data()));

    VkSurfaceFormatKHR selectedFormat = surfaceFormats[0];

#ifdef HDR
// todo 研究下hdr
    std::vector<VkFormat> preferredImageFormats = {
            VK_FORMAT_A2B10G10R10_UNORM_PACK32,
    };
#else
    std::vector<VkFormat> preferredImageFormats = {
            VK_FORMAT_B8G8R8A8_SRGB,
            VK_FORMAT_R8G8B8A8_SRGB,
            VK_FORMAT_A8B8G8R8_SRGB_PACK32,
    };
#endif

    for (auto& availableFormat : surfaceFormats) {
        if (std::find(preferredImageFormats.begin(), preferredImageFormats.end(), availableFormat.format) != preferredImageFormats.end()) {
            selectedFormat = availableFormat;
            break;
        }
    }

    color_format_ = selectedFormat.format;
    color_space_ = selectedFormat.colorSpace;

}


VkResult VulkanSwapChain::CreateSwapChain(uint32_t *width, uint32_t *height)
{
	VkSwapchainKHR oldSwapchain = swap_chain_;

    // todo 还需要考虑swapChain是被重新创建而非新建的情况，尽管这在手机上应该不常见吧....
    // 获取 surface 的能力集
    VkSurfaceCapabilitiesKHR surfCaps;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device_.physical_device_, surface_, &surfCaps));

    // 获取支持的 present 模式
    uint32_t presentModeCount;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device_.physical_device_, surface_, &presentModeCount, nullptr));
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device_.physical_device_, surface_, &presentModeCount, presentModes.data()));

    if (surfCaps.currentExtent.width == (uint32_t)-1)
    {
        settings_.extent.width = *width;
        settings_.extent.height = *height;
    }
    else
    {
        settings_.extent = surfCaps.currentExtent;
        *width = surfCaps.currentExtent.width;
        *height = surfCaps.currentExtent.height;
    }

    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (!settings_.vsync)
    {
        for (size_t i = 0; i < presentModeCount; i++)
        {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
            if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
            {
                swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
        }
    }

    uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
    if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
    {
        desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
    }

    VkSurfaceTransformFlagsKHR preTransform;
    if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfCaps.currentTransform;
    }

    		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags) {
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapchainCI = {};
		swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCI.pNext = NULL;
		swapchainCI.surface = surface_;
		swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCI.imageFormat = color_format_;
		swapchainCI.imageColorSpace = color_space_;
		swapchainCI.imageExtent = { settings_.extent.width, settings_.extent.height };
		swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchainCI.imageArrayLayers = 1;
		swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCI.queueFamilyIndexCount = 0;
		swapchainCI.pQueueFamilyIndices = NULL;
		swapchainCI.presentMode = swapchainPresentMode;
		swapchainCI.oldSwapchain = oldSwapchain;
		// Setting clipped to VK_TRUE allows the implementation to discard rendering outside the surface area
		swapchainCI.clipped = VK_TRUE;
		swapchainCI.compositeAlpha = compositeAlpha;

		// Set additional usage flag for blitting from the swapchain images if supported
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(device_.physical_device_, color_format_, &formatProps);
		if ((formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_TRANSFER_SRC_BIT_KHR) || (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT)) {
			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		VK_CHECK(vkCreateSwapchainKHR(device_, &swapchainCI, nullptr, &swap_chain_));

		// If an existing swap chain is re-created, destroy the old swap chain
		// This also cleans up all the presentable images
		if (oldSwapchain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < image_count_; i++)
			{
				vkDestroyImageView(device_, buffers_[i].view, nullptr);
			}
			vkDestroySwapchainKHR(device_, oldSwapchain, nullptr);
		}
		VK_CHECK(vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count_, NULL));

		// Get the swap chain images
		images_.resize(image_count_);
		VK_CHECK(vkGetSwapchainImagesKHR(device_, swap_chain_, &image_count_, images_.data()));

		// Get the swap chain buffers containing the image and imageview
		buffers_.resize(image_count_);
		for (uint32_t i = 0; i < image_count_; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = NULL;
			colorAttachmentView.format = color_format_;
			colorAttachmentView.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;

			buffers_[i].image = images_[i];

			colorAttachmentView.image = buffers_[i].image;

			VK_CHECK(vkCreateImageView(device_, &colorAttachmentView, nullptr, &buffers_[i].view));
		}
	return VK_SUCCESS;
}
