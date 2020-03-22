#ifndef VULKAN_HELLO_WORLD_VULKANTEXTURE_H
#define VULKAN_HELLO_WORLD_VULKANTEXTURE_H

#include <vulkan/vulkan.h>

class VulkanTexture {
public:
    VkImage image;
    VkSampler sampler;
    VkDeviceMemory memory;
    VkImageView imageView;
    uint32_t width;
    uint32_t height;
};

#endif //VULKAN_HELLO_WORLD_VULKANTEXTURE_H
