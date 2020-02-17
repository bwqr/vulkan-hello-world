#include <algorithm>
#include "VulkanModel.h"
#include "VulkanShader.h"
#include "VulkanHelper.h"

VulkanModel::VulkanModel(const std::vector<Vertex> &v, const std::vector<uint32_t> &i, uint32_t indexOffset) {
    vertices = v;
    indices.reserve(i.size());

    std::for_each(i.begin(), i.end(), [&](uint32_t index) { indices.emplace_back(index + indexOffset); });

    vertexBufferSize = sizeof(Vertex) * vertices.size();
    indexBufferSize = sizeof(uint32_t) * indices.size();
}

void VulkanModel::createDescriptorSets(const VkDevice &device, const VkDescriptorPool &pool,
                                       const VkDescriptorSetLayout &setLayout, uint32_t imageCount,
                                       VkDeviceSize uboSize, VkDescriptorBufferInfo bufferInfo) {
    std::vector<VkDescriptorSetLayout> layouts(imageCount, setLayout);
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pool;
    allocateInfo.descriptorSetCount = layouts.size();
    allocateInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(layouts.size());

    VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocateInfo, descriptorSets.data()))

    for (uint32_t i = 0; i < layouts.size(); i++) {
        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = descriptorSets[i];
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0,
                               nullptr);

        bufferInfo.offset += uboSize;
    }

}

