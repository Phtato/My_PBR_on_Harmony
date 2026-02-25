#include "render/include/vulkan_base.h"

#include <iostream>
#include <set>
#include <stdexcept>

#include "VulkanConfig.h"
#include "render/include/common.h"

VkResult VulkanBase::InitVulkan(uint32_t width, uint32_t height)
{
    VK_CHECK(this->CreateInstance());

    if (settings.enableValida) {
        // todo 鸿蒙现在是否支持了？
    }

    VK_CHECK(SelectPhysicalDevice());
    VK_CHECK(CreateLogicalDevice());
    VK_CHECK(CreateSurface());
    this->initSurface();
    swapChain = std::make_unique<VulkanSwapChain>(instance_, settings);

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












