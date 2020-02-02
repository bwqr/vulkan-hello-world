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

using namespace vtr;

class VulkanHandler {
public:
    VkExtent2D windowExtent;

    VulkanDevice device;

    VulkanSwapChain swapChain;

    VkRenderPass renderPass;

    VkCommandPool commandPool;

    std::vector<VkFramebuffer> framebuffers;

    VulkanHandler() = default;

    explicit VulkanHandler(WindowManager *windowManager);

    void resizeCallback(VkExtent2D extent);

private:
    WindowManager *windowManager;

    VkInstance instance;

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
};


#endif //VULKAN_TRY_VULKANHANDLER_H
