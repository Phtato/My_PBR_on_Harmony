#ifndef VKNDKEXAMPLE_CAMERA_H
#define VKNDKEXAMPLE_CAMERA_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "UniformBuffer.h"
#include "render/include/config.h"

class Camera
{
private:
    glm::vec3 position_{0.0f, 0.0f, 3.0f};
    glm::vec3 front_{0.0f, 0.0f, -1.0f};
    glm::vec3 up_{0.0f, 1.0f, 0.0f};
    glm::vec3 right_{1.0f, 0.0f, 0.0f};

    float yaw_ = -90.0f;
    float pitch_ = 0.0f;
    float movement_speed_ = 2.5f;
    float mouse_sensitivity_ = 0.1f;

    float fov_ = 45.0f;
    float near_ = 0.1f;
    float far_ = 100.0f;

    int width_ = 1260;
    int height_ = 2506;

    void UpdateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front.y = sin(glm::radians(pitch_));
        front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
        front_ = glm::normalize(front);
        right_ = glm::normalize(glm::cross(front_, glm::vec3(0.0f, 1.0f, 0.0f)));
        up_ = glm::normalize(glm::cross(right_, front_));
    }

public:
    Camera(Settings settings) {
        width_ = settings.extent.width;
        height_ = settings.extent.height;
        // 初始化相机向量
        UpdateCameraVectors();
    }
    ~Camera() = default;

    // 禁止拷贝和移动
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    Camera(Camera&&) = delete;
    Camera& operator=(Camera&&) = delete;


    void SetPosition(const glm::vec3& position) {
        position_ = position;
    }

    glm::vec3 GetPosition() const {
        return position_;
    }

    void SetViewportSize(int width, int height) {
        width_ = width;
        height_ = height;
    }

    void MoveForward(float deltaTime) {
        float velocity = movement_speed_ * deltaTime;
        position_ += front_ * velocity;
    }

    void MoveBack(float deltaTime) {
        float velocity = movement_speed_ * deltaTime;
        position_ -= front_ * velocity;
    }

    void MoveLeft(float deltaTime) {
        float velocity = movement_speed_ * deltaTime;
        position_ -= right_ * velocity;
    }

    void MoveRight(float deltaTime) {
        float velocity = movement_speed_ * deltaTime;
        position_ += right_ * velocity;
    }

    void MoveUp(float deltaTime) {
        float velocity = movement_speed_ * deltaTime;
        position_ += up_ * velocity;
    }

    void MoveDown(float deltaTime) {
        float velocity = movement_speed_ * deltaTime;
        position_ -= up_ * velocity;
    }

    void Rotate(float deltaX, float deltaY) {
        yaw_ += deltaX * mouse_sensitivity_;
        pitch_ += deltaY * mouse_sensitivity_;
        if (pitch_ > 89.0f) pitch_ = 89.0f;
        if (pitch_ < -89.0f) pitch_ = -89.0f;
        UpdateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const {
        return glm::lookAt(position_, position_ + front_, up_);
    }

    glm::mat4 GetProjectionMatrix() const {
        return glm::perspective(glm::radians(fov_),
                               static_cast<float>(width_) / static_cast<float>(height_),
                               near_, far_);
    }

    CameraUBO GetUBO() const {
        return CameraUBO(GetViewMatrix(), GetProjectionMatrix(), position_);
    }
};


#endif //VKNDKEXAMPLE_CAMERA_H