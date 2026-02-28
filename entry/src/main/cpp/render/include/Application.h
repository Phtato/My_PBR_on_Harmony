#ifndef VKNDKEXAMPLE_APPLICATION_H
#define VKNDKEXAMPLE_APPLICATION_H
#include <iostream>
#include <memory>
#include <ostream>

#include "vulkanBase/vulkan_base.h"


class Application
{
private:
    std::unique_ptr<VulkanBase> vulkan_base_;

public:
    Application(){
        vulkan_base_ = std::make_unique<VulkanBase>();

        auto res = vulkan_base_->InitVulkan(DEFAULT_SETTINGS);

        if (res != VK_SUCCESS)
            std::cout << "Vulkan init failed" << std::endl;
        else
            std::cout << "Vulkan init success" << std::endl;



    }
};


#endif //VKNDKEXAMPLE_APPLICATION_H