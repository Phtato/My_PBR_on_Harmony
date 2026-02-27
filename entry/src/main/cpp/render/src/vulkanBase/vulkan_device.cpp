#include "render/include/vulkanBase/vulkan_device.h"
#include "render/include/common.h"

#include <stdexcept>

VulkanDevice::~VulkanDevice()
{
    if (command_pool_) {
        vkDestroyCommandPool(logical_device_, command_pool_, nullptr);
    }
    if (logical_device_) {
        vkDestroyDevice(logical_device_, nullptr);
    }
}

VkResult VulkanDevice::SelectPhysicalDevice(VkInstance instance)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(deviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()));

    if (deviceCount == 0)
    {
        throw std::runtime_error("no gpu");
    }

    // 选择第一个物理设备，显然不太可能能在手机上找到第二个GPU
    physical_device_ = devices[0];

    // 存储物理设备的属性、特性、内存属性
    vkGetPhysicalDeviceProperties(physical_device_, &properties_);
    vkGetPhysicalDeviceFeatures(physical_device_, &features_);
    vkGetPhysicalDeviceMemoryProperties(physical_device_, &memory_properties_);

    // 获取队列族属性
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queueFamilyCount, nullptr);
    queue_family_properties_.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device_, &queueFamilyCount, queue_family_properties_.data());

    return VK_SUCCESS;
}

uint32_t VulkanDevice::GetQueueFamilyIndex(VkQueueFlagBits queueFlags) const
{
    // 对于计算队列，尝试找到专用的计算队列（不支持图形的）
    if (queueFlags & VK_QUEUE_COMPUTE_BIT)
    {
        for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties_.size()); i++) {
            if ((queue_family_properties_[i].queueFlags & queueFlags) &&
                ((queue_family_properties_[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                return i;
            }
        }
    }

    // 对于其他队列类型，返回第一个支持请求标志的队列族
    for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties_.size()); i++) {
        if (queue_family_properties_[i].queueFlags & queueFlags) {
            return i;
        }
    }

    throw std::runtime_error("Could not find a matching queue family index");
}

VkResult VulkanDevice::CreateLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures,
                                            const std::vector<const char*>& enabledExtensions,
                                            VkQueueFlags requestedQueueTypes)
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};
    const float defaultQueuePriority = 1.0f;

    // Graphics queue
    if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT) {
        queue_family_indices_.graphics = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queue_family_indices_.graphics;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        queueCreateInfos.push_back(queueInfo);
    } else {
        queue_family_indices_.graphics = 0;
    }

    // Dedicated compute queue
    if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT) {
        queue_family_indices_.compute = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
        if (queue_family_indices_.compute != queue_family_indices_.graphics) {
            VkDeviceQueueCreateInfo queueInfo{};
            queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueInfo.queueFamilyIndex = queue_family_indices_.compute;
            queueInfo.queueCount = 1;
            queueInfo.pQueuePriorities = &defaultQueuePriority;
            queueCreateInfos.push_back(queueInfo);
        }
    } else {
        queue_family_indices_.compute = queue_family_indices_.graphics;
    }

    // 创建逻辑设备
    std::vector<const char*> deviceExtensions(enabledExtensions);
    deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

    if (!deviceExtensions.empty()) {
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    }

    VkResult result = vkCreateDevice(physical_device_, &deviceCreateInfo, nullptr, &logical_device_);

    if (result == VK_SUCCESS) {
        command_pool_ = CreateCommandPool(queue_family_indices_.graphics);
        enabled_features_ = enabledFeatures;
    }

    return result;
}

uint32_t VulkanDevice::GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound) const
{
    for (uint32_t i = 0; i < memory_properties_.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((memory_properties_.memoryTypes[i].propertyFlags & properties) == properties) {
                if (memTypeFound) {
                    *memTypeFound = true;
                }
                return i;
            }
        }
        typeBits >>= 1;
    }

    if (memTypeFound) {
        *memTypeFound = false;
        return 0;
    } else {
        throw std::runtime_error("Could not find a matching memory type");
    }
}

VkCommandPool VulkanDevice::CreateCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags)
{
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = queueFamilyIndex;
    cmdPoolInfo.flags = createFlags;
    VkCommandPool cmdPool;
    VK_CHECK(vkCreateCommandPool(logical_device_, &cmdPoolInfo, nullptr, &cmdPool));
    return cmdPool;
}
