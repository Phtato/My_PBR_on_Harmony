//
// Created by 新垣つつ on 2026/3/6.
//

#include "render/include/Material.h"
#include <stdexcept>

#include "render/include/common.h"

Material::Material(VkDevice device)
    : device_(device)
{
}

Material::~Material()
{
    Cleanup();
}

Material::Material(Material&& other) noexcept
    : device_(other.device_),
      pipelineLayout_(std::move(other.pipelineLayout_)),
      pipeline_(std::move(other.pipeline_)),
      descriptorLayout_(std::move(other.descriptorLayout_)),
      descriptorSet_(other.descriptorSet_),
      textures_(std::move(other.textures_)),
      paramBuffer_(std::move(other.paramBuffer_)),
      params_(other.params_)
{
    other.device_ = VK_NULL_HANDLE;
    other.descriptorSet_ = VK_NULL_HANDLE;
}

Material& Material::operator=(Material&& other) noexcept
{
    if (this != &other) {
        Cleanup();
        device_ = other.device_;
        pipelineLayout_ = std::move(other.pipelineLayout_);
        pipeline_ = std::move(other.pipeline_);
        descriptorLayout_ = std::move(other.descriptorLayout_);
        descriptorSet_ = other.descriptorSet_;
        textures_ = std::move(other.textures_);
        paramBuffer_ = std::move(other.paramBuffer_);
        params_ = other.params_;

        other.device_ = VK_NULL_HANDLE;
        other.descriptorSet_ = VK_NULL_HANDLE;
    }
    return *this;
}

std::unique_ptr<Material> Material::CreateStandard(
    VkDevice device,
    VulkanDescriptorPool& descriptorPool,
    VkRenderPass renderPass,
    VkPipelineCache pipelineCache,
    const std::vector<uint32_t>& vertShaderCode,
    const std::vector<uint32_t>& fragShaderCode)
{
    // TODO: 实现标准 PBR 材质创建
    // 
    // 流程：
    // 1. 使用 VulkanShaderUtils 从 SPIR-V 代码创建 ShaderModule
    //    auto vertModule = VulkanShaderUtils::CreateShaderModule(device, vertShaderCode);
    //    auto fragModule = VulkanShaderUtils::CreateShaderModule(device, fragShaderCode);
    //
    // 2. 创建 DescriptorLayout（定义资源接口）
    //    std::vector<VkDescriptorSetLayoutBinding> bindings = { ...
    //      {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ...},
    //      {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ...},
    //    };
    //    auto descriptorLayout = std::make_unique<VulkanDescriptorLayout>(device, bindings);
    //
    // 3. 创建 PipelineLayout
    //    auto pipelineLayout = std::make_unique<VulkanPipelineLayout>(
    //      device, {descriptorLayout->get()});
    //
    // 4. 创建 GraphicsPipeline
    //    VulkanGraphicsPipeline::CreateInfo info{
    //      .layout = pipelineLayout->get(),
    //      .renderPass = renderPass,
    //      .vertShader = vertShaderCode,
    //      .fragShader = fragShaderCode,
    //      ...
    //    };
    //    auto pipeline = std::make_unique<VulkanGraphicsPipeline>(device, info);
    //
    // 5. 从全局 Pool 分配 DescriptorSet
    //    VkDescriptorSet set = descriptorPool.Allocate(descriptorLayout->get());
    //
    // 6. 初始化纹理和 UBO
    //
    return nullptr;
}

void Material::SetAlbedo(const VulkanImage& albedoTex)
{
    // TODO
}

void Material::SetNormalMap(const VulkanImage& normalTex)
{
    // TODO
}

void Material::SetMetallicRoughnessMap(const VulkanImage& mrTex)
{
    // TODO
}

void Material::SetPBRParams(const PBRParams& params)
{
    // TODO
}

void Material::UpdateDescriptors()
{
    // TODO
}

void Material::Cleanup()
{
    // TODO
}
