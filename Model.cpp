#include "Model.h"

Model::Model(VertexSet *vs) {
    vertexSet = vs;
}

VkDescriptorSet &Model::getDescriptorSet(uint32_t index) {
    return descriptorSets[index];
}

VkDeviceSize Model::updateVBuffer(VulkanBuffer *vBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                          VkDeviceSize dynamicAlignment) {
    vbInfo.vBuffer = vBuffer;
    vbInfo.offset = offset;
    vbInfo.size = dynamicAlignment * imageCount;
    return vbInfo.size;
}
