#include "VulkanSwapChain.h"
#include "VulkanHelper.h"

void VulkanSwapChain::initSwapChain(VulkanDevice *device, const VkSurfaceKHR &surface, const VkExtent2D &extent) {
    this->vulkanDevice = device;
    this->surface = surface;
    this->windowExtent = extent;

    createSwapChain();
    createImageViews();
}

void VulkanSwapChain::createSwapChain() {
    SwapChainSupportDetails swapChainSupportDetails = vtr::querySwapChainSupports(vulkanDevice->physicalDevice,
                                                                                  surface);

    VkSurfaceFormatKHR surfaceFormatKhr = chooseSwapSurfaceFormat(swapChainSupportDetails.formats);
    VkPresentModeKHR presentModeKhr = chooseSwapPresentMode(swapChainSupportDetails.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupportDetails.capabilities);

    imageCount = swapChainSupportDetails.capabilities.minImageCount + 1;

    if (swapChainSupportDetails.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupportDetails.capabilities.maxImageCount) {
        imageCount = swapChainSupportDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageExtent = extent;
    createInfo.imageColorSpace = surfaceFormatKhr.colorSpace;
    createInfo.imageFormat = surfaceFormatKhr.format;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices &queueFamilyIndices = vulkanDevice->queueFamilyIndices;

    if (queueFamilyIndices.graphicsFamily.value() != queueFamilyIndices.presentFamily.value()) {
        uint32_t familyIndices[] = {queueFamilyIndices.graphicsFamily.value(),
                                    queueFamilyIndices.presentFamily.value()};
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = familyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupportDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentModeKhr;
    createInfo.clipped = VK_TRUE;

    VK_CHECK_RESULT(vkCreateSwapchainKHR(vulkanDevice->logicalDevice, &createInfo, nullptr, &swapChain))

    vkGetSwapchainImagesKHR(vulkanDevice->logicalDevice, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(vulkanDevice->logicalDevice, swapChain, &imageCount, images.data());
    format = surfaceFormatKhr.format;
    extent = extent;
}

void VulkanSwapChain::createImageViews() {
    imageViews.resize(images.size());

    for (uint32_t i = 0; i < imageViews.size(); ++i) {
        createImageView(images[i], format, VK_IMAGE_ASPECT_COLOR_BIT,
                        1, &imageViews[i]);
    }
}

VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &surfaceFormats) {
    for (const auto &format : surfaceFormats) {
        if (format.format == VK_FORMAT_A8B8G8R8_UNORM_PACK32 && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return surfaceFormats[0];
}

VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &presentModes) {
    for (const auto &presentMode: presentModes) {
        if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return presentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities) {
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX) {
        return surfaceCapabilities.currentExtent;
    } else {
        VkExtent2D extent;
        extent.width = std::max(surfaceCapabilities.minImageExtent.width,
                                std::min(surfaceCapabilities.maxImageExtent.width, windowExtent.width));

        extent.height = std::max(surfaceCapabilities.minImageExtent.height,
                                 std::min(surfaceCapabilities.maxImageExtent.height, windowExtent.height));

        return extent;
    }

}

void VulkanSwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
                                      uint32_t mipLevels, VkImageView *imageView) {
    VkImageViewCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image = image;
    createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.subresourceRange.aspectMask = aspectFlags;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = mipLevels;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;
    createInfo.format = format;

    VK_CHECK_RESULT(vkCreateImageView(vulkanDevice->logicalDevice, &createInfo, nullptr, imageView))
}

void VulkanSwapChain::resizeCallback(VkExtent2D extent2D) {
    windowExtent = extent2D;
    resizeCleanup();

    createSwapChain();
    createImageViews();
}

void VulkanSwapChain::resizeCleanup() {
    for (const auto &imageView: imageViews) {
        vkDestroyImageView(vulkanDevice->logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(vulkanDevice->logicalDevice, swapChain, nullptr);
}
