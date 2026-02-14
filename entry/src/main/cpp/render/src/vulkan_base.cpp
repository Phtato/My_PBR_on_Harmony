#include "render/include/vulkan_base.h"

#include <iostream>
#include <set>
#include <stdexcept>

#include "VulkanConfig.h"
#include "render/include/common.h"

VkResult VulkanBase::InitVulkan()
{
    VK_CHECK(this->CreateInstance());

    if (settings.enableValida) {
        // todo 鸿蒙现在是否支持了？
    }

    VK_CHECK(this->SelectPhysicalDevice());
    VK_CHECK(this->CreateLogicalDevice());
    VK_CHECK(this->CreateSurface());
    this->initSurface();

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

    if (this->settings.enableValida)
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

VkResult VulkanBase::SelectPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data()));

    if (deviceCount == 0)
    {
        throw std::runtime_error("no gpu");
    }
    // 显然你不可能在手机上找到两个gpu
    physicalDevice_ = devices[0];

    return VK_SUCCESS;
}

uint32_t VulkanBase::getQueueFamilyIndex(VkQueueFlagBits queueFlags) const
{
    if (queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++) {
            if ((queueFamilies[i].queueFlags & queueFlags) && ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                return i;
                break;
            }
        }
    }

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++) {
        if (queueFamilies[i].queueFlags & queueFlags) {
            return i;
            break;
        }
    }

    throw std::runtime_error("Could not find a matching queue family index");
}

VkResult VulkanBase::CreateLogicalDevice()
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, nullptr);

    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueFamilyCount, queueFamilies.data());

    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
    queueInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueInfo.pQueuePriorities = &queuePriority;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    queueCreateInfos.push_back(queueInfo);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    VkPhysicalDeviceFeatures enabledFeatures{};
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

    // todo 这块儿还是不是很理解，后续再看看
    std::vector<const char*> deviceExtensions;
    deviceExtensions.push_back("VK_KHR_SWAPCHAIN_EXTENSION_NAME");
    deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    return vkCreateDevice(physicalDevice_, &deviceCreateInfo, nullptr, &logicalDevice_);
}

VkResult VulkanBase::CreateSurface()
{
    const VkSurfaceCreateInfoOHOS create_info{
        .sType = VK_STRUCTURE_TYPE_SURFACE_CREATE_INFO_OHOS,
        .pNext = nullptr,
        .flags = 0,
        .window = VulkanConfig::getInstance().getWindow()
    };

   return vkCreateSurfaceOHOS(instance_, &create_info, nullptr, &surface);
}

void VulkanBase::initSurface()
{
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice_, &queueCount, queueProps.data());

    std::vector<VkBool32> supportsPresent(queueCount);
    for (uint32_t i = 0; i < queueCount; i++)
    {
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice_, i, surface, &supportsPresent[i]);
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

    queueNodeIndex = graphicsQueueNodeIndex;

    // Get list of supported surface formats
    uint32_t formatCount;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface, &formatCount, NULL));

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice_, surface, &formatCount, surfaceFormats.data()));

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

    colorFormat = selectedFormat.format;
    colorSpace = selectedFormat.colorSpace;

}

VkResult VulkanBase::CreateSwapChain()
{
// todo 还需要考虑swapChain是被重新创建而非新建的情况，尽管这在手机上并不常见
    VkSurfaceCapabilitiesKHR surfCaps;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice_, surface, &surfCaps));

}









