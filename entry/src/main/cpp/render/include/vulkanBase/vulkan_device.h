//
// Created by bilibili on 2026/2/25.
//

#ifndef VKNDKEXAMPLE_VULKAN_DEVICE_H
#define VKNDKEXAMPLE_VULKAN_DEVICE_H

#include <vector>
#include <vulkan/vulkan.h>

class VulkanDevice
{
public:
    VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
    VkDevice logical_device_ = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties_;
    VkPhysicalDeviceFeatures features_;
    VkPhysicalDeviceFeatures enabled_features_;
    VkPhysicalDeviceMemoryProperties memory_properties_;
    std::vector<VkQueueFamilyProperties> queue_family_properties_;
    VkCommandPool command_pool_ = VK_NULL_HANDLE;

    struct {
        uint32_t graphics;
        uint32_t compute;
    } queue_family_indices_;

    VulkanDevice() = default;
    ~VulkanDevice();

    operator VkDevice() const { return logical_device_; }

    VkResult SelectPhysicalDevice(VkInstance instance);

    uint32_t GetQueueFamilyIndex(VkQueueFlagBits queueFlags) const;

    VkResult CreateLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures,
                                  const std::vector<const char*>& enabledExtensions,
                                  VkQueueFlags requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

    uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) const;

    VkCommandPool CreateCommandPool(uint32_t queueFamilyIndex,
                                     VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
};

#endif //VKNDKEXAMPLE_VULKAN_DEVICE_H
