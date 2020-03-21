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
#include <chrono>

class Camera {
public:

    std::vector<VirtualBufferInfo> vbInfos;

    struct {
        glm::mat4 view;
        glm::mat4 proj;
    } ubo;

    Camera() = default;

    void update(size_t index);

    void updateVBuffer(VulkanBuffer *uboVBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                               VkDeviceSize dynamicAlignment);

    void resizeCallback(VkExtent2D windowExtent);
private:
    float aspect;

    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;
    float fovy;


};


#endif //VULKAN_HELLO_WORLD_CAMERA_H
