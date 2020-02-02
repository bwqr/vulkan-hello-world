#ifndef VULKAN_TRY_VULKANDEFS_H
#define VULKAN_TRY_VULKANDEFS_H

#include <vector>
#include <vulkan/vulkan_core.h>

namespace vtr {
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    const static std::vector<const char *> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
    };

    static std::vector<const char *> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    const static bool enableValidationLayers = true;

    const static uint32_t MAX_FRAMES_IN_FLIGHT = 2;
}

#endif //VULKAN_TRY_VULKANDEFS_H
