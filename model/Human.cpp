#include "Human.h"

Human::Human(VertexSet *vs) : Model(vs) {}

void Human::update(size_t index) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    static float k = .2;

    ubo.model = glm::rotate(glm::mat4(1.0f), k * time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    vbInfo.updateData(&ubo.model, sizeof(ubo) * index, sizeof(ubo));
}

VkResult Human::createDescriptorSets(const VkDescriptorPool &pool, const VkDescriptorSetLayout &setLayout,
                                     VkDeviceSize imageCount, VkDescriptorType descriptorType) {
    std::vector<VkDescriptorSetLayout> layouts(imageCount, setLayout);
    VkDescriptorSetAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.descriptorPool = pool;
    allocateInfo.descriptorSetCount = layouts.size();
    allocateInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(layouts.size());

    auto res = vkAllocateDescriptorSets(vbInfo.vBuffer->device, &allocateInfo, descriptorSets.data());

    size_t imageSize = vbInfo.size / imageCount;

    for (uint32_t i = 0; i < layouts.size(); i++) {
        VkDescriptorBufferInfo bufferInfo;

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
