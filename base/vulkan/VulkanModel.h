#ifndef VULKAN_TRY_VULKANMODEL_H
#define VULKAN_TRY_VULKANMODEL_H


#include <vector>
#include <cstdint>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>

enum VtrModelField {
    VTR_MODEL_VERTEX,
    VTR_MODEL_INDEX
};

struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;

    static std::array<VkVertexInputAttributeDescription, 2> attributeDescription() {

        std::array<VkVertexInputAttributeDescription, 2> descriptions = {};

        descriptions[0].offset = offsetof(Vertex, pos);
        descriptions[0].location = 0;
        descriptions[0].binding = 0;
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        descriptions[1].offset = offsetof(Vertex, color);
        descriptions[1].location = 1;
        descriptions[1].binding = 0;
        descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;

        return descriptions;
    };

    static VkVertexInputBindingDescription bindingDescription() {
        VkVertexInputBindingDescription description = {};

        description.binding = 0;
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        description.stride = sizeof(Vertex);

        return description;
    };
};

class VulkanModel {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    UniformBufferObject ubo;

    std::vector<VkDescriptorSet> descriptorSets;

    VkDeviceSize vertexBufferSize;
    VkDeviceSize indexBufferSize;

    VulkanModel(const std::vector<Vertex> &v, const std::vector<uint32_t> &i);

    void
    createDescriptorSets(const VkDevice &device, const VkDescriptorPool &pool, const VkDescriptorSetLayout &setLayout,
                         uint32_t imageCount, VkDescriptorType descriptorType, VkDeviceSize uboSize,
                         const std::vector<VkDescriptorBufferInfo> &bufferInfos);

private:
};


#endif //VULKAN_TRY_VULKANMODEL_H
