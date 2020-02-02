#ifndef VULKAN_TRY_VULKANSWAPCHAIN_H
#define VULKAN_TRY_VULKANSWAPCHAIN_H


#include "VulkanDevice.h"

class VulkanSwapChain {
public:
    std::vector<VkImageView> imageViews;

    VkSwapchainKHR swapChain;

    VkFormat format;

    uint32_t imageCount;

    VulkanSwapChain() = default;

    void initSwapChain(VulkanDevice *device, const VkSurfaceKHR& surface, const VkExtent2D& extent);

    void resizeCallback(VkExtent2D extent2D);

private:
    VulkanDevice *vulkanDevice;

    VkSurfaceKHR surface;

    std::vector<VkImage> images;

    VkExtent2D windowExtent;

    void createSwapChain();

    void createImageViews();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);
    VkPresentModeKHR  chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities);

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void resizeCleanup();
};


#endif //VULKAN_TRY_VULKANSWAPCHAIN_H
