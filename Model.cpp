#include "Model.h"

Model::Model(VertexSet *vs) {
    vertexSet = vs;
}

void Model::updateVBuffer(VulkanBuffer *vBuffer, VkDeviceSize baseOffset, VkDeviceSize imageOffset,
                          VkDeviceSize imageCount, VkDeviceSize dynamicAlignment) {
    VirtualBufferInfo vbInfo = {};
    vbInfo.vBuffer = vBuffer;
    vbInfo.offset = baseOffset;
    vbInfo.size = dynamicAlignment;

    vbInfos.reserve(imageCount);

    for (size_t i = 0; i < imageCount; i++) {
        vbInfos.push_back(vbInfo);
        vbInfo.offset += imageOffset;
    }
}
