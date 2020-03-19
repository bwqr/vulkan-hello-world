#ifndef VULKAN_HELLO_WORLD_CAMERA_H
#define VULKAN_HELLO_WORLD_CAMERA_H


#include <glm/glm.hpp>
#include <vector>
#include "base/vulkan/VulkanBuffer.h"
#include "VirtualBufferInfo.h"
#include "Model.h"

class Camera : Model {
public:

    struct {
        glm::mat4 view;
        glm::mat4 proj;
    } ubo;

    std::vector<VkDescriptorSet> descriptorSets;

    Camera();

    VkResult createDescriptorSets(const VkDescriptorPool &pool, const VkDescriptorSetLayout &setLayout,
                                  VkDeviceSize imageCount, VkDescriptorType descriptorType) override;

    void update(size_t index) override;
};


#endif //VULKAN_HELLO_WORLD_CAMERA_H
