#ifndef VKNDKEXAMPLE_SCENE_H
#define VKNDKEXAMPLE_SCENE_H
#include <memory>
#include <vector>

#include "Camera.h"
#include "Renderable.h"
#include "UniformBuffer.h"

class Scene
{
private:
    std::unique_ptr<Camera> m_camera;
    std::vector<std::shared_ptr<Renderable>> m_renderables;
    std::vector<glm::vec3> m_initialPositions;  // 存储每个物体的初始位置
    LightUBO m_mainLight{
        .position = glm::vec3(2.0f, 2.0f, 2.0f),
        .intensity = 1.0f,
        .color = glm::vec3(1.0f, 1.0f, 1.0f),
        .type = 0  // 点光
    };
    float m_autoRotationAngle = 0.0f;  // 自动旋转角度

public:
    Scene() = default;
    ~Scene() = default;

    // 禁止拷贝和移动（Camera 是 unique_ptr）
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;


};


#endif //VKNDKEXAMPLE_SCENE_H