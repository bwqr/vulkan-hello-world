

#ifndef VULKAN_HELLO_WORLD_VULKANBUFFER_H
#define VULKAN_HELLO_WORLD_VULKANBUFFER_H


#include <vulkan/vulkan.h>
#include <cstring>
#include <cassert>

class VulkanBuffer {
public:
    VkDevice device = VK_NULL_HANDLE;

    VkBuffer buffer = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;
    VkDeviceSize alignment = 0;

    VulkanBuffer() = default;

    void copy(const void *bufferData, VkDeviceSize offset, VkDeviceSize _size) {
        assert(mapped);
        memcpy(static_cast<char *>(mapped) + offset, bufferData, _size);
    }

    VkResult map(VkDeviceSize offset, VkDeviceSize _size) {
        return vkMapMemory(device, memory, offset, _size, flags, &mapped);
    }

    void unmap() {
        if (mapped != nullptr) {
            vkUnmapMemory(device, memory);
        }
    }

private:
    VkMemoryMapFlags flags = 0;

    void *mapped = nullptr;
};


#endif //VULKAN_HELLO_WORLD_VULKANBUFFER_H
