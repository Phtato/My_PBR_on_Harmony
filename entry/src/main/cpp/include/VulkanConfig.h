//
// Created by bilibili on 2025/11/24.
// Global configuration management for Vulkan NDK
//

#ifndef VKNDKEXAMPLE_VULKAN_CONFIG_H
#define VKNDKEXAMPLE_VULKAN_CONFIG_H

#include <string>
#include <mutex>
#include <rawfile/raw_file_manager.h>
#include <ace/xcomponent/native_interface_xcomponent.h>

class VulkanConfig {
public:
    // Singleton instance
    static VulkanConfig& getInstance();
    
    // Delete copy constructor and assignment operator
    VulkanConfig(const VulkanConfig&) = delete;
    VulkanConfig& operator=(const VulkanConfig&) = delete;
    
    // Getters
    const std::string& getOhosPath() const;
    NativeResourceManager* getResourceManager() const;
    OHNativeWindow* getWindow() const;
    bool isDestroyed() const;
    
    // Setters
    void setOhosPath(const std::string& path);
    void setResourceManager(NativeResourceManager* rm);
    void setWindow(OHNativeWindow* window);
    void setDestroy(bool destroy);
    
    // Reset all to default values
    void reset();

private:
    // Private constructor for singleton
    VulkanConfig();
    
    // Member variables
    std::string ohosPath_;
    NativeResourceManager* resourceManager_;
    OHNativeWindow* window_;
    bool isDestroy_;
    
    // Mutex for thread safety
    mutable std::mutex mutex_;
};

#endif // VKNDKEXAMPLE_VULKAN_CONFIG_H
