//
// Created by 新垣つつ on 2026/3/6.
//

#ifndef VKNDKEXAMPLE_MATERIAL_H
#define VKNDKEXAMPLE_MATERIAL_H

#include <memory>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <glm/glm.hpp>

#include "vulkanBase/vulkan_buffer.h"
#include "vulkanBase/vulkan_image.h"
#include "vulkanBase/vulkan_pipeline_layout.h"
#include "vulkanBase/vulkan_graphics_pipeline.h"
#include "vulkanBase/vulkan_descriptor.h"
#include "vulkanBase/vulkan_shader_utils.h"
#include "ShaderLibrary.h"

// ============================================================================
//  Material
//  
//  管理单个材质的所有渲染相关资源：
//  - Pipeline（如何渲染）
//  - DescriptorLayout（资源接口）
//  - 纹理和 UBO（资源数据）
//  - DescriptorSet（资源绑定）
//
//  典型用法：
//    auto material = Material::CreateStandard(device, pool, "pbr.vert", "pbr.frag");
//    material->SetAlbedo(albedoImage);
//    material->SetMetallic(0.5f);
//    material->UpdateDescriptors();
//    
//    // 渲染时
//    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->GetPipeline());
//    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ..., material->GetDescriptorSet());
// ============================================================================
class Material {
public:
    /**
     * @brief PBR 材质参数（UBO）
     */
    struct PBRParams {
        glm::vec4 baseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};
        float     metallic = 0.0f;
        float     roughness = 1.0f;
        float     ao = 1.0f;
        float     _padding = 0.0f;
    };

    /**
     * @brief 创建标准 PBR 材质
     *
     * @param device              逻辑设备
     * @param descriptorPool      全局描述符池（从这里分配 DescriptorSet）
     * @param renderPass          渲染通道
     * @param pipelineCache       管线缓存
     * @param vertShaderCode      顶点着色器 SPIR-V 代码
     * @param fragShaderCode      片段着色器 SPIR-V 代码
     *
     * 集成流程：
     * 1. ShaderLibrary 加载 SPIR-V 文件 → std::vector<uint32_t>
     * 2. VulkanShaderUtils 创建 VkShaderModule
     * 3. Material 创建 Pipeline 和 Layout
     *
     * 示例用法：
     *   ShaderLibrary shaderLib;
     *   auto vertCode = shaderLib.LoadSPIRV("pbr.vert.spv");
     *   auto fragCode = shaderLib.LoadSPIRV("pbr.frag.spv");
     *   auto material = Material::CreateStandard(device, pool, renderPass, cache, 
     *                                            vertCode, fragCode);
     */
    static std::unique_ptr<Material> CreateStandard(
        VkDevice device,
        VulkanDescriptorPool& descriptorPool,
        VkRenderPass renderPass,
        VkPipelineCache pipelineCache,
        const std::vector<uint32_t>& vertShaderCode,
        const std::vector<uint32_t>& fragShaderCode);

    ~Material();

    // 禁用拷贝
    Material(const Material&)            = delete;
    Material& operator=(const Material&) = delete;

    // 允许移动
    Material(Material&& other) noexcept;
    Material& operator=(Material&& other) noexcept;

    // ========== 资源访问 ==========

    /**
     * @brief 获取图形管线
     */
    VkPipeline GetPipeline() const { return pipeline_->get(); }

    /**
     * @brief 获取管线布局
     */
    VkPipelineLayout GetPipelineLayout() const { return pipelineLayout_->get(); }

    /**
     * @brief 获取描述符集
     */
    VkDescriptorSet GetDescriptorSet() const { return descriptorSet_; }

    // ========== 材质参数设置 ==========

    /**
     * @brief 设置基础颜色纹理（或颜色因子）
     */
    void SetAlbedo(const VulkanImage& albedoTex);

    /**
     * @brief 设置法线贴图
     */
    void SetNormalMap(const VulkanImage& normalTex);

    /**
     * @brief 设置金属度/粗糙度纹理
     */
    void SetMetallicRoughnessMap(const VulkanImage& mrTex);

    /**
     * @brief 设置 PBR 参数（基础色、金属度、粗糙度等）
     */
    void SetPBRParams(const PBRParams& params);

    /**
     * @brief 提交参数更新到 GPU（调用 Flush）
     */
    void UpdateDescriptors();

private:
    Material(VkDevice device);

    // 核心渲染对象
    std::unique_ptr<VulkanPipelineLayout>    pipelineLayout_;
    std::unique_ptr<VulkanGraphicsPipeline>  pipeline_;
    std::unique_ptr<VulkanDescriptorLayout>  descriptorLayout_;
    VkDescriptorSet descriptorSet_ = VK_NULL_HANDLE;

    // 材质资源
    std::vector<std::unique_ptr<VulkanImage>>  textures_;  // albedo, normal, mr 等
    std::unique_ptr<VulkanBuffer>              paramBuffer_;  // UBO

    // 本地 PBR 参数（用于 CPU 侧缓存）
    PBRParams params_;

    VkDevice device_ = VK_NULL_HANDLE;

    void Cleanup();
};


#endif //VKNDKEXAMPLE_MATERIAL_H
