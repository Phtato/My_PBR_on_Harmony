


#include "include/vulkanBase/vulkan_base.h"
#include "napi/native_api.h"
#include <iostream>

VulkanBase* vulkanBase;

/*
 * 这里作为入口吧
*/
extern "C" napi_value entry(napi_env env, napi_callback_info info)
{
    std::cout << "[Vulkan] Starting initialization..." << std::endl;

    vulkanBase = new VulkanBase();

    VkResult result = vulkanBase->InitVulkan(1260, 2506);
    if (result == VK_SUCCESS) {
        std::cout << "[Vulkan] Initialization completed successfully" << std::endl;
    } else {
        std::cout << "[Vulkan] Initialization FAILED, error code: " << result << std::endl;
    }



    return nullptr;
}