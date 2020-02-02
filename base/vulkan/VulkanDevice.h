#ifndef VULKAN_TRY_VULKANDEVICE_H
#define VULKAN_TRY_VULKANDEVICE_H


#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <optional>
#include "VulkanDefs.h"

using namespace vtr;

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};


class VulkanDevice {

public:
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    QueueFamilyIndices queueFamilyIndices;

    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VulkanDevice() = default;

    ~VulkanDevice();

    void initVulkanDevice(const VkInstance &instance, const VkSurfaceKHR &surface);
private:
    VkInstance instance;

    VkSurfaceKHR surface;

    void pickPhysicalDevice();

    bool isDeviceSuitable(const VkPhysicalDevice &device);

    QueueFamilyIndices findQueueFamily(const VkPhysicalDevice &device);

    bool checkDeviceExtensionSupport(const VkPhysicalDevice &device);

    void createLogicalDevice();
};


#endif //VULKAN_TRY_VULKANDEVICE_H
