//
// Created by 新垣つつ on 2026/3/2.
//

#ifndef VKNDKEXAMPLE_VULKAN_SHADER_UTILS_H
#define VKNDKEXAMPLE_VULKAN_SHADER_UTILS_H

#include <vulkan/vulkan_core.h>
#include <string>
#include <cstdint>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <iostream>

/**
 * @brief Vulkan Shader 工具类
 *
 * 提供 SPIR-V 着色器模块加载功能，支持文件路径和内存加载两种方式
 *
 * 典型用法：
 * ```cpp
 * // 方式1：加载 ShaderModule（底层）
 * auto module = VulkanShaderUtils::LoadSPIRV(device, "shader.spv");
 * // ... 使用后需要手动销毁
 * vkDestroyShaderModule(device, module, nullptr);
 *
 * // 方式2：直接加载 PipelineShaderStage（推荐）
 * auto vertStage = VulkanShaderUtils::LoadShaderStage(device, "vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
 * auto fragStage = VulkanShaderUtils::LoadShaderStage(device, "frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
 * ```
 */
class VulkanShaderUtils {
public:
    /**
     * @brief 从文件路径加载 SPIR-V 着色器
     *
     * @param device Vulkan 逻辑设备
     * @param filename SPIR-V 文件名（相对于 shaders 目录）
     * @return VkShaderModule 着色器模块句柄
     *
     * @throws std::runtime_error 文件读取失败或创建着色器模块失败
     *
     * @note 返回的 VkShaderModule 需要手动调用 vkDestroyShaderModule 销毁
     * @note 文件路径会根据平台自动拼接（鸿蒙平台需设置 shaderBasePath）
     */
    static VkShaderModule LoadSPIRV(VkDevice device, const std::string& filename) {
        // 拼接完整路径（需要根据你的项目配置调整）
        std::string fullPath = shaderBasePath + filename;

        std::ifstream file(fullPath, std::ios::ate | std::ios::binary);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shader file: " + fullPath);
        }

        size_t fileSize = static_cast<size_t>(file.tellg());
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return LoadSPIRV(device, reinterpret_cast<const uint32_t*>(buffer.data()), fileSize);
    }

    /**
     * @brief 从内存加载 SPIR-V 着色器
     *
     * @param device Vulkan 逻辑设备
     * @param code SPIR-V 字节码指针（uint32_t 数组）
     * @param codeSize 字节码大小（字节数）
     * @return VkShaderModule 着色器模块句柄
     *
     * @throws std::runtime_error 创建着色器模块失败
     *
     * @note 适用于鸿蒙 rawfile 或嵌入式资源场景
     * @note 返回的 VkShaderModule 需要手动调用 vkDestroyShaderModule 销毁
     * @note codeSize 必须是 4 的倍数（SPIR-V 要求）
     */
    static VkShaderModule LoadSPIRV(VkDevice device, const uint32_t* code, size_t codeSize) {
        if (codeSize % 4 != 0) {
            throw std::runtime_error("SPIR-V code size must be a multiple of 4");
        }

        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = codeSize;
        createInfo.pCode = code;

        VkShaderModule shaderModule;
        VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module, VkResult: " + std::to_string(result));
        }

        return shaderModule;
    }

    /**
     * @brief 加载着色器并返回 Pipeline Stage 创建信息（推荐使用）
     *
     * @param device Vulkan 逻辑设备
     * @param filename SPIR-V 文件名
     * @param stage 着色器阶段（顶点/片段等）
     * @param entryPoint 着色器入口函数名，默认 "main"
     * @return VkPipelineShaderStageCreateInfo 可直接用于创建 Pipeline
     *
     * @throws std::runtime_error 加载失败
     *
     * @note 返回结构体中的 module 需要在 Pipeline 创建后手动销毁
     */
    static VkPipelineShaderStageCreateInfo LoadShaderStage(
        VkDevice device,
        const std::string& filename,
        VkShaderStageFlagBits stage,
        const char* entryPoint = "main")
    {
        VkPipelineShaderStageCreateInfo shaderStage{};
        shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStage.stage = stage;
        shaderStage.module = LoadSPIRV(device, filename);
        shaderStage.pName = entryPoint;

        return shaderStage;
    }

    /**
     * @brief 设置着色器文件基础路径
     *
     * @param path 基础路径（如 "/data/storage/el2/base/haps/entry/files/shaders/"）
     *
     * @note 鸿蒙平台需要在运行时设置正确的路径
     */
    static void SetShaderBasePath(const std::string& path) {
        shaderBasePath = path;
        if (!shaderBasePath.empty() && shaderBasePath.back() != '/') {
            shaderBasePath += '/';
        }
    }

private:
    inline static std::string shaderBasePath = "./shaders/"; // 默认路径
};

#endif //VKNDKEXAMPLE_VULKAN_SHADER_UTILS_H
