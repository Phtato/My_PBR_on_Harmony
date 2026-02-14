//
// Created by bilibili on 2025/11/24.
// Implementation of global configuration management
//

#include "include/VulkanConfig.h"

// Singleton instance initialization
VulkanConfig& VulkanConfig::getInstance() {
    static VulkanConfig instance;
    return instance;
}

// Constructor
VulkanConfig::VulkanConfig()
    : ohosPath_(""),
      resourceManager_(nullptr),
      window_(nullptr),
      isDestroy_(false) {
}

// Getters
const std::string& VulkanConfig::getOhosPath() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return ohosPath_;
}

NativeResourceManager* VulkanConfig::getResourceManager() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return resourceManager_;
}

OHNativeWindow* VulkanConfig::getWindow() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return window_;
}

bool VulkanConfig::isDestroyed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return isDestroy_;
}

// Setters
void VulkanConfig::setOhosPath(const std::string& path) {
    std::lock_guard<std::mutex> lock(mutex_);
    ohosPath_ = path;
}

void VulkanConfig::setResourceManager(NativeResourceManager* rm) {
    std::lock_guard<std::mutex> lock(mutex_);
    resourceManager_ = rm;
}

void VulkanConfig::setWindow(OHNativeWindow* window) {
    std::lock_guard<std::mutex> lock(mutex_);
    window_ = window;
}

void VulkanConfig::setDestroy(bool destroy) {
    std::lock_guard<std::mutex> lock(mutex_);
    isDestroy_ = destroy;
}

// Reset all to default values
void VulkanConfig::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    ohosPath_.clear();
    resourceManager_ = nullptr;
    window_ = nullptr;
    isDestroy_ = false;
}
