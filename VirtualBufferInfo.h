

#ifndef VULKAN_HELLO_WORLD_VIRTUALBUFFERINFO_H
#define VULKAN_HELLO_WORLD_VIRTUALBUFFERINFO_H

#include <vector>
#include "base/vulkan/VulkanBuffer.h"

class VirtualBufferInfo {
public:
    VirtualBufferInfo() = default;

    void updateData(void *data, VkDeviceSize _offset, VkDeviceSize _size);

    VulkanBuffer *vBuffer;
    VkDeviceSize offset;
    VkDeviceSize size;
};

#endif //VULKAN_HELLO_WORLD_VIRTUALBUFFERINFO_H
