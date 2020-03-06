#include <algorithm>
#include "VulkanModel.h"
#include "VulkanShader.h"
#include "VulkanHelper.h"

VulkanModel::VulkanModel(const std::vector<Vertex> &v, const std::vector<uint32_t> &i) {
    vertices = v;
    indices = i;

    vertexBufferSize = sizeof(Vertex) * vertices.size();
    indexBufferSize = sizeof(uint32_t) * indices.size();
}

void VulkanModel::createDescriptorSets(const VkDevice &device, const VkDescriptorPool &pool,
                                       const VkDescriptorSetLayout &setLayout, uint32_t imageCount,
                                       const VkDescriptorType descriptorType,
                                       VkDeviceSize uboSize, const std::vector<VkDescriptorBufferInfo> &bufferInfos) {
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
        writeDescriptorSet.descriptorType = descriptorType;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.pBufferInfo = &bufferInfos[i];

        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0,
                               nullptr);
    }
}

