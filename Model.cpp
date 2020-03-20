#include "Model.h"

Model::Model(VertexSet *vs) {
    vertexSet = vs;
}

VkDeviceSize Model::updateVBuffer(VulkanBuffer *vBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                          VkDeviceSize dynamicAlignment) {
    vbInfo.vBuffer = vBuffer;
    vbInfo.offset = offset;
    vbInfo.size = dynamicAlignment * imageCount;
    return vbInfo.size;
}
