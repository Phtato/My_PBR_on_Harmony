#ifndef VKNDKEXAMPLE_APPLICATION_H
#define VKNDKEXAMPLE_APPLICATION_H
#include <iostream>
#include <memory>
#include <ostream>
#include <chrono>
#include <thread>

#include "vulkanBase/vulkan_base.h"
#include <hilog/log.h>

#undef LOG_DOMAIN
#undef LOG_TAG
#define LOG_DOMAIN 0x1145
#define LOG_TAG "Application"

// TODO: 声明 SimpleMesh 类（实现后）
// class SimpleMesh;

class Application
{
private:
    std::unique_ptr<VulkanBase> vulkan_base_;
    // TODO: std::unique_ptr<SimpleMesh> triangle_mesh_;
    
    bool should_run = true;
    uint32_t current_frame_index_ = 0;

    // ===== 初始化方法 =====
    void initializeScene() {
        // TODO: 创建三角形网格
        // triangle_mesh_ = std::make_unique<SimpleMesh>(*vulkan_base_->GetDevice());
        
        OH_LOG_INFO(LOG_APP, "Scene initialized");
    }
    
    // ===== 渲染循环的三个阶段 =====
    
    // 阶段1: 获取交换链图像
    VkResult AcquireFrame() {
        // TODO: 实现
        // - 等待 inFlightFence
        // - 重置 Fence
        // - vkAcquireNextImageKHR() 获取图像索引
        // - 重置命令缓冲区
        return VK_SUCCESS;
    }
    
    // 阶段2: 记录绘制命令
    VkResult RecordCommands() {
        // TODO: 实现
        // - vkBeginCommandBuffer()
        // - vkCmdBeginRenderPass()
        // - vkCmdBindPipeline()
        // - vkCmdBindVertexBuffers()
        // - vkCmdDraw() / vkCmdDrawIndexed()
        // - vkCmdEndRenderPass()
        // - vkEndCommandBuffer()
        return VK_SUCCESS;
    }
    
    // 阶段3: 提交并呈现
    VkResult PresentFrame() {
        // TODO: 实现
        // - vkQueueSubmit()
        // - vkQueuePresentKHR()
        // - 更新 current_frame_index_
        return VK_SUCCESS;
    }

public:
    Application(){
        vulkan_base_ = std::make_unique<VulkanBase>();

        auto res = vulkan_base_->InitVulkan(DEFAULT_SETTINGS);

        if (res != VK_SUCCESS) {
            OH_LOG_ERROR(LOG_APP, "Vulkan init failed");
            throw std::runtime_error("Vulkan initialization failed");
        } else {
            OH_LOG_INFO(LOG_APP, "Vulkan init success");
        }
        
        // 初始化场景资源
        this->initializeScene();
    }
    
    ~Application() {
        should_run = false;
        // TODO: 清理资源（三角形网格等）
    }
    
    // 禁用拷贝和移动
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(Application&&) = delete;

    void run() {
        should_run = true;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto lastFrameTime = startTime;
        
        // 帧率限制：60 FPS = 16.67ms per frame
        constexpr float TARGET_FPS = 60.0f;
        constexpr float FRAME_TIME = 1.0f / TARGET_FPS;  // 秒
        
        uint64_t frame_count = 0;

        while (should_run) {
            // TODO: 检查销毁信号
            // if (VulkanConfig::getInstance().isDestroyed()) break;
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                currentTime - lastFrameTime).count();

            // 帧率限制
            float timeToWait = FRAME_TIME - deltaTime;
            if (timeToWait > 0.0f) {
                int millisecondsToWait = static_cast<int>(timeToWait * 1000.0f);
                std::this_thread::sleep_for(std::chrono::milliseconds(millisecondsToWait));
            }

            // 重新获取时间
            currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(
                currentTime - lastFrameTime).count();
            lastFrameTime = currentTime;

            //=========================================
            // 主渲染循环三阶段
            
            VkResult result = AcquireFrame();
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                // TODO: 处理交换链重建
                continue;
            }
            
            result = RecordCommands();
            if (result != VK_SUCCESS) {
                OH_LOG_ERROR(LOG_APP, "Failed to record commands");
                break;
            }
            
            result = PresentFrame();
            if (result != VK_SUCCESS) {
                OH_LOG_ERROR(LOG_APP, "Failed to present frame");
                break;
            }
            
            //=========================================
            
            frame_count++;
            
            // 每秒打印一次FPS
            if (frame_count % 60 == 0) {
                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - startTime).count();
                float fps = (frame_count * 1000.0f) / elapsed_ms;
                OH_LOG_INFO(LOG_APP, "FPS: %.1f", fps);
            }
        }
        
        OH_LOG_INFO(LOG_APP, "Render loop ended. Total frames: %llu", frame_count);
    }
    
    void Stop() { should_run = false; }
    
    VulkanBase* GetVulkanBase() { return vulkan_base_.get(); }
};


#endif //VKNDKEXAMPLE_APPLICATION_H