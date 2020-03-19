#ifndef VULKAN_HELLO_WORLD_MODEL_H
#define VULKAN_HELLO_WORLD_MODEL_H


#include "VertexSet.h"

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
    } ubo;

    std::vector<VkDescriptorSet> descriptorSets;

    VirtualBufferInfo vbInfo;

    Model(VertexSet *vs);

    virtual void update(size_t index) = 0;

    virtual VkResult createDescriptorSets(const VkDescriptorPool &pool, const VkDescriptorSetLayout &setLayout,
                                          VkDeviceSize imageCount, VkDescriptorType descriptorType) = 0;

    VkDeviceSize updateVBuffer(VulkanBuffer *vBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                       VkDeviceSize dynamicAlignment);

    VkDescriptorSet &getDescriptorSet(uint32_t index);
};


#endif //VULKAN_HELLO_WORLD_MODEL_H
