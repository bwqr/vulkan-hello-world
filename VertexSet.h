

#ifndef VULKAN_HELLO_WORLD_VERTEXSET_H
#define VULKAN_HELLO_WORLD_VERTEXSET_H

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <array>
#include <vector>
#include "VirtualBufferInfo.h"

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static std::array<VkVertexInputAttributeDescription, 3> attributeDescription() {

        std::array<VkVertexInputAttributeDescription, 3> descriptions = {};

        descriptions[0].offset = offsetof(Vertex, pos);
        descriptions[0].location = 0;
        descriptions[0].binding = 0;
        descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;

        descriptions[1].offset = offsetof(Vertex, color);
        descriptions[1].location = 1;
        descriptions[1].binding = 0;
        descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;

        descriptions[2].offset = offsetof(Vertex, texCoord);
        descriptions[2].location = 2;
        descriptions[2].binding = 0;
        descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;

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

class VertexSet {
public:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    VirtualBufferInfo vertexVBInfo;
    VirtualBufferInfo indexVBInfo;

    uint32_t vertexOffset;
    uint32_t indexOffset;

    void updateVertexVBInfo(VkDeviceSize bufferOffset);
    void updateIndexVBInfo(VkDeviceSize bufferOffset);
private:

};


#endif //VULKAN_HELLO_WORLD_VERTEXSET_H
