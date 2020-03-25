#ifndef VULKAN_HELLO_WORLD_MODEL_H
#define VULKAN_HELLO_WORLD_MODEL_H


#include "../VertexSet.h"
#include "../Camera.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

class Model {
public:
    VertexSet *vertexSet;
    struct {
        glm::mat4 model;
        glm::int32 texIndex;
    } ubo;

    std::vector<VirtualBufferInfo> vbInfos;

    Model(VertexSet *vs, glm::int32 texIndex);

    virtual void updateUBO(size_t index) = 0;

    void
    updateVBuffer(VulkanBuffer *vBuffer, VkDeviceSize baseOffset, VkDeviceSize imageOffset, VkDeviceSize imageCount,
                  VkDeviceSize dynamicAlignment);

    glm::vec3 position = {0.0f, 0.0f, 0.0f};

    float speed = 1;

    glm::vec3 scale = {1, 1, 1};
};


#endif //VULKAN_HELLO_WORLD_MODEL_H
