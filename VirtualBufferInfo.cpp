#include "VirtualBufferInfo.h"
#include "base/vulkan/VulkanHelper.h"

void VirtualBufferInfo::updateData(void *data, VkDeviceSize _offset, VkDeviceSize _size) {
    vBuffer->copy(data, offset + _offset, _size);
}
