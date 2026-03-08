//
// Created by 新垣つつ on 2026/3/6.
//

#ifndef VKNDKEXAMPLE_VULKAN_GRAPHICS_PIPELINE_H
#define VKNDKEXAMPLE_VULKAN_GRAPHICS_PIPELINE_H

#include <vector>
#include <vulkan/vulkan_core.h>

// ============================================================================
//  VulkanGraphicsPipeline
//  管理 VkPipeline（图形管线），包含 Shader、Rasterization 等配置。
//
//  典型用法：
//    VulkanGraphicsPipeline pipeline(device, {
//        .layout = pipelineLayout.get(),
//        .renderPass = renderPass,
//        .subpass = 0,
//        .vertShader = vertSpv,      // SPIR-V 二进制数据
//        .fragShader = fragSpv,
//    });
//
//    vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.get());
// ============================================================================
class VulkanGraphicsPipeline {
public:
    /**
     * @brief 图形管线创建参数结构体
     */
    struct CreateInfo {
        VkPipelineLayout       layout;                  ///< 必需：Pipeline Layout
        VkRenderPass          renderPass;              ///< 必需：所属 RenderPass
        uint32_t              subpass = 0;             ///< VkRenderPass 中的子通道索引

        // Shader 相关
        std::vector<uint32_t> vertShader;              ///< 顶点着色器 SPIR-V 二进制
        std::vector<uint32_t> fragShader;              ///< 片段着色器 SPIR-V 二进制
        std::vector<uint32_t> geomShader;              ///< （可选）几何着色器
        std::vector<uint32_t> tessCtrlShader;          ///< （可选）曲面细分控制着色器
        std::vector<uint32_t> tessEvalShader;          ///< （可选）曲面细分计算着色器

        // Vertex Input
        std::vector<VkVertexInputBindingDescription>   vertexBindings;
        std::vector<VkVertexInputAttributeDescription> vertexAttributes;

        // Rasterization
        VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
        VkFrontFace     frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        VkBool32        depthTest = VK_TRUE;
        VkBool32        depthWrite = VK_TRUE;
        VkCompareOp     depthCompareOp = VK_COMPARE_OP_LESS;

        // Viewport & Scissor（如为 dynamic，在 command buffer 中设置）
        VkBool32        dynamicViewport = VK_TRUE;
        VkBool32        dynamicScissor = VK_TRUE;

        // Color Blending
        VkBool32        blendEnable = VK_FALSE;
        VkBlendFactor   srcColorBlend = VK_BLEND_FACTOR_SRC_ALPHA;
        VkBlendFactor   dstColorBlend = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        VkBlendOp       colorBlendOp = VK_BLEND_OP_ADD;
    };

    /**
     * @brief 构造函数：创建 VkPipeline
     *
     * @param device     逻辑设备
     * @param info       管线创建参数
     * @throws std::runtime_error 着色器编译或管线创建失败
     */
    VulkanGraphicsPipeline(VkDevice device, const CreateInfo& info);

    ~VulkanGraphicsPipeline();

    // 禁用拷贝
    VulkanGraphicsPipeline(const VulkanGraphicsPipeline&)            = delete;
    VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;

    // 允许移动
    VulkanGraphicsPipeline(VulkanGraphicsPipeline&& other) noexcept;
    VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&& other) noexcept;

    /**
     * @brief 获取底层 VkPipeline
     */
    VkPipeline get() const { return pipeline_; }

private:
    VkDevice  device_   = VK_NULL_HANDLE;
    VkPipeline pipeline_ = VK_NULL_HANDLE;

    void Cleanup();
};


#endif //VKNDKEXAMPLE_VULKAN_GRAPHICS_PIPELINE_H
