#include "Camera.h"
#include "base/vulkan/VulkanHelper.h"

Camera::Camera() : Model(nullptr) {
}

void Camera::update(size_t index) {
}

VkResult Camera::createDescriptorSets(const VkDescriptorPool &pool, const VkDescriptorSetLayout &setLayout,
                                      VkDeviceSize imageCount, VkDescriptorType descriptorType) {
    std::vector<VkDescriptorSetLayout> layouts(imageCount, setLayout);
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pool;
    allocateInfo.descriptorSetCount = layouts.size();
    allocateInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(layouts.size());

    auto res = vkAllocateDescriptorSets(vbInfo.vBuffer->device, &allocateInfo, descriptorSets.data());

    for (uint32_t i = 0; i < layouts.size(); i++) {
        VkDescriptorBufferInfo bufferInfo;

        size_t imageSize = vbInfo.size / imageCount;

        bufferInfo.buffer = vbInfo.vBuffer->buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = imageSize;

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = descriptorSets[i];
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorType = descriptorType;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(vbInfo.vBuffer->device, 1, &writeDescriptorSet, 0,
                               nullptr);
    }

    return res;
}