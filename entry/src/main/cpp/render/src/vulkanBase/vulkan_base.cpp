#include "render/include/vulkanBase/vulkan_base.h"

#include <array>
#include <iostream>
#include <set>
#include <stdexcept>

#include "VulkanConfig.h"
#include "render/include/common.h"

VkResult VulkanBase::InitVulkan(Settings settings)
{
	settings_ = settings;
    std::cout << "[Vulkan] Creating instance..." << std::endl;
    VK_CHECK(CreateInstance());

    if (settings_.enableValidation) {
        // todo 鸿蒙现在是否支持了？
    }

    std::cout << "[Vulkan] Selecting physical device..." << std::endl;
    device_ = std::make_unique<VulkanDevice>();
    VK_CHECK(device_->SelectPhysicalDevice(instance_));

    VkPhysicalDeviceFeatures enabledFeatures{};
    std::vector<const char*> enabledExtensions;
    VK_CHECK(device_->CreateLogicalDevice(enabledFeatures, enabledExtensions));

    GetDeviceCaps();

    std::cout << "[Vulkan] Creating swap chain..." << std::endl;
    VK_CHECK(prepare());

    return VK_SUCCESS;
}

VkResult VulkanBase::CreateInstance()
{
    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Vulkan on harmony",
        .apiVersion = VK_API_VERSION_1_4
    };

    std::vector<const char*> instanceExtensions;
    instanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
    instanceExtensions.push_back(VK_OHOS_SURFACE_EXTENSION_NAME);

    if (this->settings_.enableValidation)
    {
        instanceExtensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
    }

    VkInstanceCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &appInfo
    };

    createInfo.enabledExtensionCount = static_cast<int32_t>(instanceExtensions.size());
    createInfo.ppEnabledExtensionNames = instanceExtensions.data();

    // todo 换一个allocator
    return vkCreateInstance(&createInfo, nullptr, &instance_);
}


VkResult VulkanBase::GetDeviceCaps()
{
	/*
	Graphics queue
	*/
	vkGetDeviceQueue(device_->logical_device_, device_->queue_family_indices_.graphics, 0, &queue_);

	/*
		Suitable depth format
	*/
	std::vector<VkFormat> depthFormats = { VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT, VK_FORMAT_D16_UNORM };
	VkBool32 validDepthFormat = false;
	for (auto& format : depthFormats) {
		VkFormatProperties formatProps;
		vkGetPhysicalDeviceFormatProperties(device_->physical_device_, format, &formatProps);
		if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
			depth_format_ = format;
			validDepthFormat = true;
			break;
		}
	}
	return VK_SUCCESS;
}

VkResult VulkanBase::prepare()
{
    VkDevice& device = device_->logical_device_;
    VkPhysicalDevice& physicalDevice = device_->physical_device_;

    swap_chain_ = std::make_unique<VulkanSwapChain>(instance_, settings_, *device_);

    	/*
		Command pool
	*/
	VkCommandPoolCreateInfo cmdPoolInfo = {};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = swap_chain_->queue_node_index_;
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VK_CHECK(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &cmd_pool_));

	/*
		Render pass
	*/

	if (settings_.multiSampling) {
		std::array<VkAttachmentDescription, 4> attachments = {};

		// Multisampled attachment that we render to
		attachments[0].format = swap_chain_->color_format_;
		attachments[0].samples = settings_.sampleCount;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// This is the frame buffer attachment to where the multisampled image
		// will be resolved to and which will be presented to the swapchain
		attachments[1].format = swap_chain_->color_format_;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Multisampled depth attachment we render to
		attachments[2].format = depth_format_;
		attachments[2].samples = settings_.sampleCount;
		attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[2].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Depth resolve attachment
		attachments[3].format = depth_format_;
		attachments[3].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[3].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[3].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[3].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 2;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Resolve attachment reference for the color attachment
		VkAttachmentReference resolveReference = {};
		resolveReference.attachment = 1;
		resolveReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorReference;
		// Pass our resolve attachments to the sub pass
		subpass.pResolveAttachments = &resolveReference;
		subpass.pDepthStencilAttachment = &depthReference;

		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCI = {};
		renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCI.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassCI.pAttachments = attachments.data();
		renderPassCI.subpassCount = 1;
		renderPassCI.pSubpasses = &subpass;
		renderPassCI.dependencyCount = 2;
		renderPassCI.pDependencies = dependencies.data();
		VK_CHECK(vkCreateRenderPass(device, &renderPassCI, nullptr, &render_pass_));
	}
	else {
		std::array<VkAttachmentDescription, 2> attachments = {};
		// Color attachment
		attachments[0].format = swap_chain_->color_format_;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		// Depth attachment
		attachments[1].format = depth_format_;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.pDepthStencilAttachment = &depthReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		// Subpass dependencies for layout transitions
		std::array<VkSubpassDependency, 2> dependencies;

		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
		dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

		VkRenderPassCreateInfo renderPassCI{};
		renderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCI.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassCI.pAttachments = attachments.data();
		renderPassCI.subpassCount = 1;
		renderPassCI.pSubpasses = &subpassDescription;
		renderPassCI.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassCI.pDependencies = dependencies.data();
		VK_CHECK(vkCreateRenderPass(device, &renderPassCI, nullptr, &render_pass_));
	}

	/*
		Pipeline cache
	*/
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	VK_CHECK(vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipeline_cache_));

	/*
		Frame buffer
	*/
	std::cout << "[Vulkan] Creating framebuffers..." << std::endl;
	setupFrameBuffer();
}

void VulkanBase::setupFrameBuffer()
{
	/*
	MSAA
	*/
	if (settings_.multiSampling) {
		// Check if device supports requested sample count for color and depth frame buffer
		//assert((deviceProperties.limits.framebufferColorSampleCounts >= sampleCount) && (deviceProperties.limits.framebufferDepthSampleCounts >= sampleCount));

		VkImageCreateInfo imageCI{};
		imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCI.imageType = VK_IMAGE_TYPE_2D;
		imageCI.format = swap_chain_->color_format_;
		imageCI.extent.width = settings_.extent.width;
		imageCI.extent.height = settings_.extent.height;
		imageCI.extent.depth = 1;
		imageCI.mipLevels = 1;
		imageCI.arrayLayers = 1;
		imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCI.samples = settings_.sampleCount;
		imageCI.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VK_CHECK(vkCreateImage(device_->logical_device_, &imageCI, nullptr, &multisampleTarget.color.image));

		VkMemoryRequirements memReqs;
		vkGetImageMemoryRequirements(device_->logical_device_, multisampleTarget.color.image, &memReqs);
		VkMemoryAllocateInfo memAllocInfo{};
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memReqs.size;
		VkBool32 lazyMemTypePresent;
		memAllocInfo.memoryTypeIndex = device_->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, &lazyMemTypePresent);
		if (!lazyMemTypePresent) {
			memAllocInfo.memoryTypeIndex = device_->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		}
		VK_CHECK(vkAllocateMemory(device_->logical_device_, &memAllocInfo, nullptr, &multisampleTarget.color.memory));
		vkBindImageMemory(device_->logical_device_, multisampleTarget.color.image, multisampleTarget.color.memory, 0);

		// Create image view for the MSAA target
		VkImageViewCreateInfo imageViewCI{};
		imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCI.image = multisampleTarget.color.image;
		imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCI.format = swap_chain_->color_format_;
		imageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
		imageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
		imageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
		imageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
		imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCI.subresourceRange.levelCount = 1;
		imageViewCI.subresourceRange.layerCount = 1;
		VK_CHECK(vkCreateImageView(device_->logical_device_, &imageViewCI, nullptr, &multisampleTarget.color.view));

		// Depth target
		imageCI.imageType = VK_IMAGE_TYPE_2D;
		imageCI.format = depth_format_;
		imageCI.extent.width = settings_.extent.width;
		imageCI.extent.height = settings_.extent.height;
		imageCI.extent.depth = 1;
		imageCI.mipLevels = 1;
		imageCI.arrayLayers = 1;
		imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCI.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCI.samples = settings_.sampleCount;
		imageCI.usage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VK_CHECK(vkCreateImage(device_->logical_device_, &imageCI, nullptr, &multisampleTarget.depth.image));

		vkGetImageMemoryRequirements(device_->logical_device_, multisampleTarget.depth.image, &memReqs);
		memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAllocInfo.allocationSize = memReqs.size;
		memAllocInfo.memoryTypeIndex = device_->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT, &lazyMemTypePresent);
		if (!lazyMemTypePresent) {
			memAllocInfo.memoryTypeIndex = device_->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		}
		VK_CHECK(vkAllocateMemory(device_->logical_device_, &memAllocInfo, nullptr, &multisampleTarget.depth.memory));
		vkBindImageMemory(device_->logical_device_, multisampleTarget.depth.image, multisampleTarget.depth.memory, 0);

		// Create image view for the MSAA target
		imageViewCI.image = multisampleTarget.depth.image;
		imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCI.format = depth_format_;
		imageViewCI.components.r = VK_COMPONENT_SWIZZLE_R;
		imageViewCI.components.g = VK_COMPONENT_SWIZZLE_G;
		imageViewCI.components.b = VK_COMPONENT_SWIZZLE_B;
		imageViewCI.components.a = VK_COMPONENT_SWIZZLE_A;
		imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		imageViewCI.subresourceRange.levelCount = 1;
		imageViewCI.subresourceRange.layerCount = 1;
		VK_CHECK(vkCreateImageView(device_->logical_device_, &imageViewCI, nullptr, &multisampleTarget.depth.view));
	}


	// Depth/Stencil attachment is the same for all frame buffers

	VkImageCreateInfo image = {};
	image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image.pNext = NULL;
	image.imageType = VK_IMAGE_TYPE_2D;
	image.format = depth_format_;
	image.extent = { settings_.extent.width, settings_.extent.height, 1 };
	image.mipLevels = 1;
	image.arrayLayers = 1;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	image.flags = 0;

	VkMemoryAllocateInfo mem_alloc = {};
	mem_alloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	mem_alloc.pNext = NULL;
	mem_alloc.allocationSize = 0;
	mem_alloc.memoryTypeIndex = 0;

	VkImageViewCreateInfo depthStencilView = {};
	depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthStencilView.pNext = NULL;
	depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthStencilView.format = depth_format_;
	depthStencilView.flags = 0;
	depthStencilView.subresourceRange = {};
	depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	depthStencilView.subresourceRange.baseMipLevel = 0;
	depthStencilView.subresourceRange.levelCount = 1;
	depthStencilView.subresourceRange.baseArrayLayer = 0;
	depthStencilView.subresourceRange.layerCount = 1;

	VkMemoryRequirements memReqs;
	VK_CHECK(vkCreateImage(device_->logical_device_, &image, nullptr, &depthStencil.image));
	vkGetImageMemoryRequirements(device_->logical_device_, depthStencil.image, &memReqs);
	mem_alloc.allocationSize = memReqs.size;
	mem_alloc.memoryTypeIndex = device_->GetMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(device_->logical_device_, &mem_alloc, nullptr, &depthStencil.mem));
	VK_CHECK(vkBindImageMemory(device_->logical_device_, depthStencil.image, depthStencil.mem, 0));

	depthStencilView.image = depthStencil.image;
	VK_CHECK(vkCreateImageView(device_->logical_device_, &depthStencilView, nullptr, &depthStencil.view));

	//

	VkImageView attachments[4];

	if (settings_.multiSampling) {
		attachments[0] = multisampleTarget.color.view;
		attachments[2] = multisampleTarget.depth.view;
		attachments[3] = depthStencil.view;
	}
	else {
		attachments[1] = depthStencil.view;
	}

	VkFramebufferCreateInfo frameBufferCI{};
	frameBufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCI.pNext = NULL;
	frameBufferCI.renderPass = render_pass_;
	frameBufferCI.attachmentCount = settings_.multiSampling ? 4 :2;
	frameBufferCI.pAttachments = attachments;
	frameBufferCI.width = settings_.extent.width;
	frameBufferCI.height = settings_.extent.height;
	frameBufferCI.layers = 1;

	// Create frame buffers for every swap chain image
	std::cout << "[SetupFrameBuffer] Creating " << swap_chain_->image_count_ << " framebuffers..." << std::endl;
	frame_buffers_.resize(swap_chain_->image_count_);
	for (uint32_t i = 0; i < frame_buffers_.size(); i++) {
		if (settings_.multiSampling) {
			attachments[1] = swap_chain_->buffers_[i].view;
		}
		else {
			attachments[0] = swap_chain_->buffers_[i].view;
		}
		VK_CHECK(vkCreateFramebuffer(device_->logical_device_, &frameBufferCI, nullptr, &frame_buffers_[i]));
	}
	std::cout << "[SetupFrameBuffer] All " << frame_buffers_.size() << " framebuffers created successfully" << std::endl;
}








