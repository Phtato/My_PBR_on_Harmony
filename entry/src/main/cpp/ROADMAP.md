# Vulkan Renderer Roadmap

## 当前已完成

| 类 | 内容 |
|---|---|
| `VulkanDevice` | 物理/逻辑设备、队列族、内存查询 |
| `VulkanSwapChain` | Surface、交换链、ImageView |
| `VulkanBase` | Instance、CommandPool、RenderPass、PipelineCache、Framebuffer |
| `Application` | 顶层入口 |

---

## 整体路线图

```
Phase 1 → 基础资源抽象
Phase 2 → 第一个三角形（同步 + 渲染循环）
Phase 3 → 场景/资源系统
Phase 4 → PBR 光照
Phase 5 → 高级效果（可选）
```

---

## Phase 1 — 基础资源抽象

### `VulkanBuffer`

文件：`render/include/vulkanBase/vulkan_buffer.h`

```cpp
class VulkanBuffer {
public:
    VkBuffer       buffer  = VK_NULL_HANDLE;
    VkDeviceMemory memory  = VK_NULL_HANDLE;
    VkDeviceSize   size    = 0;
    void*          mapped  = nullptr;

    static VulkanBuffer Create(VulkanDevice& device,
                               VkDeviceSize size,
                               VkBufferUsageFlags usage,
                               VkMemoryPropertyFlags props);

    void Map();
    void Unmap();
    void CopyTo(const void* data, VkDeviceSize size);
    void Flush();
    void Destroy(VkDevice device);
};
```

用于：顶点缓冲、索引缓冲、Uniform Buffer、Staging Buffer。

---

### `VulkanImage`

文件：`render/include/vulkanBase/vulkan_image.h`

```cpp
class VulkanImage {
public:
    VkImage        image   = VK_NULL_HANDLE;
    VkImageView    view    = VK_NULL_HANDLE;
    VkDeviceMemory memory  = VK_NULL_HANDLE;
    VkSampler      sampler = VK_NULL_HANDLE;  // 可选

    static VulkanImage Create(VulkanDevice& device,
                              uint32_t width, uint32_t height,
                              VkFormat format,
                              VkImageUsageFlags usage,
                              VkImageAspectFlags aspect);

    // 布局迁移辅助
    static void TransitionLayout(VkCommandBuffer cmd,
                                 VkImage image,
                                 VkImageLayout oldLayout,
                                 VkImageLayout newLayout);
    void Destroy(VkDevice device);
};
```

---

### `VulkanShader`

文件：`render/include/vulkanBase/vulkan_shader.h`

```cpp
class VulkanShader {
public:
    // 从文件路径加载
    static VkShaderModule LoadSPIRV(VkDevice device,
                                    const std::string& path);
    // 从内存加载（鸿蒙 rawfile）
    static VkShaderModule LoadSPIRV(VkDevice device,
                                    const uint32_t* code,
                                    size_t codeSize);
};
```

---

### `VulkanDescriptorPool`

文件：`render/include/vulkanBase/vulkan_descriptor.h`

```cpp
class VulkanDescriptorPool {
public:
    VulkanDescriptorPool(VkDevice device,
                         uint32_t maxSets,
                         const std::vector<VkDescriptorPoolSize>& sizes);

    ~VulkanDescriptorPool();

    VkDescriptorSet Allocate(VkDescriptorSetLayout layout);
    void Reset();

    VkDescriptorPool get() const;
};
```

负责向驱动预申请资源槽位，所有 Layout 的 Set 都从这里分配。

---

### `VulkanDescriptorLayout`

文件：`render/include/vulkanBase/vulkan_descriptor.h`

```cpp
class VulkanDescriptorLayout {
public:
    VulkanDescriptorLayout(VkDevice device,
                           const std::vector<VkDescriptorSetLayoutBinding>& bindings);

    ~VulkanDescriptorLayout();

    VkDescriptorSetLayout get() const;

    void UpdateBuffer(VkDescriptorSet set, uint32_t binding,
                      VkBuffer buffer, VkDeviceSize offset,
                      VkDeviceSize size, VkDescriptorType type);

    void UpdateImage(VkDescriptorSet set, uint32_t binding,
                     VkImageView view, VkSampler sampler,
                     VkImageLayout imgLayout);
};
```

管理一种 VkDescriptorSetLayout，并提供 UpdateBuffer/UpdateImage 便捷接口。

**典型用法：**

```cpp
// 1. 创建全局 Descriptor Pool
VulkanDescriptorPool pool(device, 64, {
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         64},
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 128},
});

// 2. 创建 Material 描述符布局
VulkanDescriptorLayout matLayout(device, {
    {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         1, VK_SHADER_STAGE_FRAGMENT_BIT},
    {1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT},
});

// 3. 从 Pool 分配 DescriptorSet
VkDescriptorSet set = pool.Allocate(matLayout.get());

// 4. 更新 Descriptor
matLayout.UpdateBuffer(set, 0, ubo, 0, sizeof(UBO));
matLayout.UpdateImage(set, 1, texView, sampler);
```

---

## Phase 2 — 第一个三角形（渲染循环 + 同步）

### 扩展 `VulkanBase`：全局基础设施

在 `VulkanBase` 中新增全局资源管理：

```cpp
class VulkanBase {
public:
    // 帧同步对象（每帧独立）
    struct FrameData {
        VkCommandBuffer cmd;
        VkSemaphore     imageAvailableSemaphore;
        VkSemaphore     renderFinishedSemaphore;
        VkFence         inFlightFence;
    };
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
    std::array<FrameData, MAX_FRAMES_IN_FLIGHT> frames_;

    // 全局资源池（所有 Material 从这里分配 DescriptorSet）
    std::unique_ptr<VulkanDescriptorPool> descriptor_pool_;

    // 获取全局资源池
    VulkanDescriptorPool& GetDescriptorPool();

private:
    VkRenderPass render_pass_;           // 全局渲染通道
    VkPipelineCache pipeline_cache_;     // 管线编译缓存
    std::vector<VkFramebuffer> frame_buffers_;
    DepthStencil depthStencil_;         // 全局深度
    MultisampleTarget multisampleTarget; // 全局 MSAA
};
```

---

### `Material`（材质）

文件：`render/include/Material.h`

```cpp
class Material {
public:
    // 创建标准 PBR 材质
    static std::unique_ptr<Material> CreateStandard(
        VkDevice device,
        VulkanDescriptorPool& descriptorPool,
        VkRenderPass renderPass,
        VkPipelineCache pipelineCache,
        const std::vector<uint32_t>& vertShaderCode,
        const std::vector<uint32_t>& fragShaderCode);

    // 资源访问
    VkPipeline GetPipeline();
    VkPipelineLayout GetPipelineLayout();
    VkDescriptorSet GetDescriptorSet();

    // 材质参数设置
    void SetAlbedo(const VulkanImage& albedoTex);
    void SetNormalMap(const VulkanImage& normalTex);
    void SetMetallicRoughnessMap(const VulkanImage& mrTex);
    void SetPBRParams(const PBRParams& params);
    void UpdateDescriptors();

private:
    // 材质专属渲染对象
    std::unique_ptr<VulkanPipelineLayout>    pipelineLayout_;
    std::unique_ptr<VulkanGraphicsPipeline>  pipeline_;
    std::unique_ptr<VulkanDescriptorLayout>  descriptorLayout_;
    
    // 资源
    VkDescriptorSet descriptorSet_;  // 从 VulkanBase.DescriptorPool 分配
    std::vector<std::unique_ptr<VulkanImage>> textures_;
    std::unique_ptr<VulkanBuffer> paramBuffer_;
};
```

**职责：** 管理单个材质的所有渲染资源和参数。

---

### `ShaderLibrary`（着色器库）

文件：`render/include/ShaderLibrary.h`

```cpp
class ShaderLibrary {
public:
    struct ShaderPair {
        std::vector<uint32_t> vertex;
        std::vector<uint32_t> fragment;
    };

    // 从鸿蒙 rawfile 或文件系统加载编译后的 SPIR-V 代码
    std::vector<uint32_t> LoadSPIRV(const std::string& filename);
    ShaderPair LoadShaderPair(const std::string& baseName);
    std::vector<uint32_t> LoadSPIRVFromPath(const std::string& filePath);
    void ClearCache();
};
```

**职责：** 高层资源加载，从鸿蒙 rawfile 或文件系统加载和缓存 SPIR-V 代码。只返回数据，不创建 VkShaderModule。

### `VulkanShaderUtils`（Vulkan 工具）

文件：`render/include/vulkanBase/vulkan_shader_utils.h`

```cpp
class VulkanShaderUtils {
public:
    // 从内存创建 VkShaderModule（底层 API 包装）
    static VkShaderModule LoadSPIRV(VkDevice device, const uint32_t* code, size_t codeSize);
    
    // 加载并直接返回 Pipeline Stage 创建信息
    static VkPipelineShaderStageCreateInfo LoadShaderStage(
        VkDevice device,
        const std::string& filename,
        VkShaderStageFlagBits stage,
        const char* entryPoint = "main");
};
```

**职责：** 底层 Vulkan API 包装，创建 VkShaderModule。

### 集成使用示例

```cpp
// 1. 加载 SPIR-V 代码
ShaderLibrary shaderLib;
auto vertCode = shaderLib.LoadSPIRV("pbr.vert.spv");     // std::vector<uint32_t>
auto fragCode = shaderLib.LoadSPIRV("pbr.frag.spv");

// 2. 创建 ShaderModule
auto vertModule = VulkanShaderUtils::LoadSPIRV(device, vertCode.data(), vertCode.size());
auto fragModule = VulkanShaderUtils::LoadSPIRV(device, fragCode.data(), fragCode.size());

// 3. 用于 Material 创建 Pipeline
auto material = Material::CreateStandard(device, pool, renderPass, cache, vertCode, fragCode);
```

---

### 扩展 `Application`：渲染循环

```cpp
class Application {
public:
    void Run();                   // 主循环入口

private:
    std::unique_ptr<ShaderLibrary> shaderLibrary_;
    std::unique_ptr<Material> material_;

    void BuildCommandBuffers();   // 录制 RenderPass 指令
    void DrawFrame();             // acquire → submit → present
    void OnResize();              // swapchain 重建
};
```

渲染循环流程：

```
vkAcquireNextImageKHR
    └── 等待 imageAvailableSemaphore
vkQueueSubmit
    ├── 等待 imageAvailableSemaphore
    └── 触发 renderFinishedSemaphore
vkQueuePresentKHR
    └── 等待 renderFinishedSemaphore
```

---

## Phase 3 — 场景 / 资源系统

### `Vertex` / `Mesh`

文件：`render/include/Mesh.h`

```cpp
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec4 tangent;
};

class Mesh {
public:
    VulkanBuffer vertexBuffer_;
    VulkanBuffer indexBuffer_;
    uint32_t     indexCount_;

    static Mesh LoadFromGLTF(VulkanDevice& device, const std::string& path);
};
```

---

### `Material`（PBR 参数）

文件：`render/include/Material.h`

```cpp
struct MaterialUBO {
    glm::vec4 baseColorFactor;
    float     metallic;
    float     roughness;
    float     ao;
    float     _padding;
};

class Material {
public:
    VulkanImage  albedo_;
    VulkanImage  normalMap_;
    VulkanImage  metallicRoughness_;
    VulkanImage  emissive_;
    VulkanBuffer ubo_;
    VkDescriptorSet descriptorSet_;
};
```

---

### `Camera`

文件：`render/include/Camera.h`

```cpp
class Camera {
public:
    glm::mat4 GetView()       const;
    glm::mat4 GetProjection() const;

    glm::vec3 position_;
    glm::vec3 target_;
    float     fov_, near_, far_;
    float     aspect_;
};
```

---

### `Scene`

文件：`render/include/Scene.h`

```cpp
class Scene {
public:
    std::vector<Mesh>     meshes_;
    std::vector<Material> materials_;
    Camera                camera_;

    void Load(const std::string& path);
    void Update(float deltaTime);
};
```

---

## Phase 4 — PBR 光照

| 模块 | 说明 |
|---|---|
| `IBLPipeline` | 预计算 irradiance cube、pre-filtered env map、BRDF LUT |
| `ShadowPass` | Shadow map（定向光 / 点光），PCF 软阴影 |
| `LightingPass` | 直接光 + IBL 合并的 PBR shading |
| `TonemapPass` | HDR → LDR，ACES / Reinhard |

### Shader Descriptor Set 布局

```glsl
// set 0 — 场景级（每帧更新一次）
layout(set=0, binding=0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    vec4 cameraPos;
    vec4 lightDir;
    vec4 lightColor;
};

// set 1 — 材质级（每个 DrawCall 绑定）
layout(set=1, binding=0) uniform MaterialUBO { ... };
layout(set=1, binding=1) uniform sampler2D albedoMap;
layout(set=1, binding=2) uniform sampler2D normalMap;
layout(set=1, binding=3) uniform sampler2D metallicRoughnessMap;

// set 2 — IBL（全局共享）
layout(set=2, binding=0) uniform samplerCube irradianceMap;
layout(set=2, binding=1) uniform samplerCube prefilteredMap;
layout(set=2, binding=2) uniform sampler2D   brdfLUT;
```

---

## Phase 5 — 高级效果（可选）

| 效果 | 关键技术 |
|---|---|
| **SSAO** | 屏幕空间环境光遮蔽，G-Buffer 法线 + 深度 |
| **Bloom** | 亮度阈值提取 → 高斯模糊 → 叠加 |
| **TAA** | 时域抗锯齿（替换当前 MSAA），历史帧 reprojection |
| **Ray Tracing** | `VK_KHR_ray_tracing_pipeline`，硬件光追 PBR |

---

## 目标目录结构

```
render/
├── include/
│   ├── Application.h
│   ├── Camera.h           ← Phase 3
│   ├── Mesh.h             ← Phase 3
│   ├── Material.h         ← Phase 2 ✓ 创建完毕
│   ├── Scene.h            ← Phase 3
│   ├── ShaderLibrary.h    ← Phase 2 ✓ 创建完毕
│   ├── config.h
│   ├── common.h
│   └── vulkanBase/
│       ├── vulkan_base.h
│       ├── vulkan_device.h
│       ├── vulkan_swap_chain.h
│       ├── vulkan_buffer.h                ← Phase 1
│       ├── vulkan_image.h                 ← Phase 1
│       ├── vulkan_shader.h                ← Phase 1
│       ├── vulkan_descriptor.h            ← Phase 1
│       ├── vulkan_pipeline_layout.h       ← Phase 2
│       └── vulkan_graphics_pipeline.h     ← Phase 2
├── src/
│   ├── Application.cpp
│   ├── Camera.cpp
│   ├── Mesh.cpp
│   ├── Material.cpp                       ← Phase 2 ✓ 创建完毕
│   ├── Scene.cpp
│   ├── ShaderLibrary.cpp                  ← Phase 2 ✓ 创建完毕
│   └── vulkanBase/
│       ├── vulkan_base.cpp
│       ├── vulkan_device.cpp
│       ├── vulkan_swap_chain.cpp
│       ├── vulkan_buffer.cpp              ← Phase 1 ✓
│       ├── vulkan_image.cpp               ← Phase 1 ✓
│       ├── vulkan_shader.cpp              ← Phase 1
│       ├── vulkan_descriptor.cpp          ← Phase 1
│       ├── vulkan_pipeline_layout.cpp     ← Phase 2
│       └── vulkan_graphics_pipeline.cpp   ← Phase 2
└── shaders/
    ├── pbr.vert
    ├── pbr.frag
    ├── shadow.vert
    ├── shadow.frag
    └── ibl_precompute.comp
```

---

## 实施顺序

```
[ Phase 1 ] VulkanBuffer ✓ → VulkanImage ✓ → VulkanShader → VulkanDescriptorLayout/Pool
      ↓
[ Phase 2 ] 帧同步对象 → VulkanPipelineLayout → VulkanGraphicsPipeline → Material ✓ → ShaderLibrary ✓ → DrawFrame 循环 → 第一个三角形
      ↓
[ Phase 3 ] Mesh (glTF) → Camera → Scene → 渲染第一个 3D 模型
      ↓
[ Phase 4 ] IBL 预计算 → ShadowPass → PBR Shading → Tonemap → 物理正确渲染
      ↓
[ Phase 5 ] SSAO → Bloom → TAA → Ray Tracing（选做）
```
