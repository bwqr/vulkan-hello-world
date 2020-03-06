#include <cstring>
#include <set>
#include "VulkanDevice.h"
#include "VulkanHelper.h"

void VulkanDevice::initVulkanDevice(const VkInstance &instance, const VkSurfaceKHR &surface) {
    this->instance = instance;
    this->surface = surface;

    pickPhysicalDevice();
    queueFamilyIndices = findQueueFamily(physicalDevice);
    createLogicalDevice();
}

void VulkanDevice::pickPhysicalDevice() {
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    for (const auto &device: physicalDevices) {
        if (isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }

    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
}

void VulkanDevice::createLogicalDevice() {
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueIndices = {queueFamilyIndices.presentFamily.value(),
                                             queueFamilyIndices.graphicsFamily.value()};

    float queuePriority = 1.0f;
    for (const auto &queueIndex: uniqueQueueIndices) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.queueFamilyIndex = queueIndex;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.enabledExtensionCount = deviceExtensions.size();
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledLayerCount = validationLayers.size();
    createInfo.ppEnabledLayerNames = validationLayers.data();
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    VK_CHECK_RESULT(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice));

    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
}

bool VulkanDevice::isDeviceSuitable(const VkPhysicalDevice &device) {
    auto queueFamilyIndices = findQueueFamily(device);
    auto extensionSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionSupported) {
        auto swapChainSupportDetails = vtr::querySwapChainSupports(device, surface);
        swapChainAdequate = !swapChainSupportDetails.presentModes.empty() && !swapChainSupportDetails.formats.empty();
    }

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    return queueFamilyIndices.isComplete() && extensionSupported && swapChainAdequate &&
           deviceFeatures.samplerAnisotropy;
}

QueueFamilyIndices VulkanDevice::findQueueFamily(const VkPhysicalDevice &device) {
    QueueFamilyIndices indices;

    uint32_t indicesCount;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &indicesCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(indicesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &indicesCount, queueFamilies.data());

    uint32_t i = 0;
    for (const auto &queueFamily: queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

bool VulkanDevice::checkDeviceExtensionSupport(const VkPhysicalDevice &device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension: availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}



VulkanDevice::~VulkanDevice() {

}
