//
// Created by 新垣つつ on 2026/3/6.
//

#ifndef VKNDKEXAMPLE_SHADER_LIBRARY_H
#define VKNDKEXAMPLE_SHADER_LIBRARY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan_core.h>

// ============================================================================
//  ShaderLibrary
//  
//  着色器资源加载库（高层）
//  
//  职责：
//  - 从鸿蒙 rawfile 或文件系统加载编译后的 SPIR-V 二进制代码
//  - 缓存已加载的 SPIR-V 代码，复用避免重复加载
//  
//  不负责：
//  - 创建 VkShaderModule（交给 VulkanShaderUtils）
//  - 编译着色器源代码（已预编译为 SPIR-V）
//
//  典型用法：
//    ShaderLibrary shaderLib;
//    auto vertCode = shaderLib.LoadSPIRV("vert.spv");   // std::vector<uint32_t>
//    auto fragCode = shaderLib.LoadSPIRV("frag.spv");
//    
//    // 然后用 VulkanShaderUtils 创建 VkShaderModule
//    auto vertModule = VulkanShaderUtils::CreateShaderModule(device, vertCode);
// ============================================================================
class ShaderLibrary {
public:
    /**
     * @brief 着色器对（顶点 + 片段）
     */
    struct ShaderPair {
        std::vector<uint32_t> vertex;
        std::vector<uint32_t> fragment;
    };

    ShaderLibrary() = default;
    ~ShaderLibrary() = default;

    /**
     * @brief 从 rawfile 加载单个 SPIR-V 着色器
     *
     * @param filename 文件名，如 "vert.spv", "frag.spv"
     * @return SPIR-V 二进制数据（32-bit unsigned 整数数组）
     * @throws std::runtime_error 文件不存在或加载失败
     *
     * 支持两种加载模式：
     * 1. 鸿蒙平台 - 从 rawfile 加载（需鸿蒙 API）
     * 2. PC 调试 - 从本地文件系统加载（开发用）
     */
    std::vector<uint32_t> LoadSPIRV(const std::string& filename);

    /**
     * @brief 加载着色器对（顶点 + 片段）
     *
     * @param baseName 基础名称，会自动添加 .vert.spv 和 .frag.spv 后缀
     *                 示例："pbr" → 加载 "pbr.vert.spv" 和 "pbr.frag.spv"
     * @return 包含 vertex 和 fragment 两个字段的结构体
     */
    ShaderPair LoadShaderPair(const std::string& baseName);

    /**
     * @brief 从完整路径加载 SPIR-V（调试用）
     *
     * @param filePath 完整文件路径
     * @return SPIR-V 二进制数据
     */
    std::vector<uint32_t> LoadSPIRVFromPath(const std::string& filePath);

    /**
     * @brief 清空缓存
     */
    void ClearCache();

private:
    // SPIR-V 代码缓存（相同文件只加载一次）
    std::unordered_map<std::string, std::vector<uint32_t>> m_cache;

    /**
     * @brief 从文件系统读取二进制文件（以 32-bit 单位）
     */
    std::vector<uint32_t> ReadBinaryFile(const std::string& filePath);

    /**
     * @brief 从鸿蒙 rawfile 读取二进制文件
     */
    std::vector<uint32_t> ReadRawFile(const std::string& filename);
};


#endif //VKNDKEXAMPLE_SHADER_LIBRARY_H
