#ifndef VULKAN_HELLO_WORLD_CAMERA_H
#define VULKAN_HELLO_WORLD_CAMERA_H


#include <glm/glm.hpp>
#include <vector>
#include "base/vulkan/VulkanBuffer.h"
#include "VirtualBufferInfo.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <chrono>

class Camera {
public:

    std::vector<VirtualBufferInfo> vbInfos;

    glm::mat4 view;
    glm::mat4 proj;

    struct {
        glm::mat4 viewProjection;
    } ubo;

    glm::vec3 eye;

    glm::vec3 viewPlaneDirection;

    Camera() = default;

    void rotateViewPlane(float dx, float dy);

    void translateViewPlane(float dx, float dy);

    void updateUBO(size_t index);

    void updateVBuffer(VulkanBuffer *uboVBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                       VkDeviceSize dynamicAlignment);

    void resizeCallback(VkExtent2D windowExtent);

private:
    float aspect;

    glm::vec3 up = {0.0f, 0.0f, 1.0f};

    glm::fquat orientation = {1.0f, 0.0f, 0.0f, 0.0f};

    void quaternionRotation(glm::vec3 axis, float rad);
};


#endif //VULKAN_HELLO_WORLD_CAMERA_H
