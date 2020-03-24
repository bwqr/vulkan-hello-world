#ifndef VULKAN_HELLO_WORLD_VULKANIMAGE_H
#define VULKAN_HELLO_WORLD_VULKANIMAGE_H

#include <vulkan/vulkan.h>

class VulkanImage {
public:
    VkImage image;
    VkSampler sampler;
    VkDeviceMemory memory;
    VkImageView imageView;
    VkImageLayout imageLayout;
    uint32_t width;
    uint32_t height;
};

#endif //VULKAN_HELLO_WORLD_VULKANIMAGE_H
