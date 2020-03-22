#ifndef VULKAN_TRY_VULKANHANDLER_H
#define VULKAN_TRY_VULKANHANDLER_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "VulkanHelper.h"
#include "../window/WindowManager.h"
#include "../window/glfw/GLFWWindowManager.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanDefs.h"
#include "VulkanBuffer.h"
#include "../../VertexSet.h"
#include "VulkanTexture.h"

using namespace vtr;

typedef void (*DataAccessCallback)(void *);

class VulkanHandler {
public:
    VkInstance instance;

    VkExtent2D windowExtent;

    VulkanDevice device;

    VulkanSwapChain swapChain;

    VkRenderPass renderPass;

    VkCommandPool commandPool;

    std::vector<VkFramebuffer> framebuffers;

    VulkanHandler() = default;

    explicit VulkanHandler(WindowManager *windowManager);

    void resizeCallback(VkExtent2D extent);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                      VulkanBuffer &vBuffer);

    void copyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size);

    void createVertexSetsBuffer(VulkanBuffer &vBuffer, std::vector<VertexSet> &vertexSets,
                                VkDeviceSize *indexOffset);

    void createTexture(VkDeviceSize imageSize, void *data, uint32_t width, uint32_t height, VulkanTexture *vTexture);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer &commandBuffer);

private:
    WindowManager *windowManager;

    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR surface;

    void initVulkan();

    void createInstance();

    void setupDebugMessenger();

    std::vector<const char *> getRequiredExtensions();

    void createSurface();

    void createRenderPass();

    void createSwapChain();

    void createFramebuffers();

    void createCommandPool();

    void resizeCleanup();

    void updateFramebufferSize(VkExtent2D extent);

    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                     VkMemoryPropertyFlags properties, VulkanTexture *vTexture);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void createTextureSampler(VkSampler *sampler);
};


#endif //VULKAN_TRY_VULKANHANDLER_H
