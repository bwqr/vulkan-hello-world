#include "VertexSet.h"

void VertexSet::updateVertexVBInfo(VkDeviceSize bufferOffset) {
    vertexVBInfo.offset = bufferOffset;
    vertexVBInfo.size = vertices.size() * sizeof(Vertex);
}

void VertexSet::updateIndexVBInfo(VkDeviceSize bufferOffset) {
    indexVBInfo.offset = bufferOffset;
    indexVBInfo.size = indices.size() * sizeof(uint32_t);
}
