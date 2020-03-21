#include "VirtualBufferInfo.h"
#include "base/vulkan/VulkanHelper.h"

void VirtualBufferInfo::updateData(void *data, VkDeviceSize _size) {
    vBuffer->copy(data, offset, _size);
}
