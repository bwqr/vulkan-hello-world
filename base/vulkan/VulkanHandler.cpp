#include <iostream>
#include "VulkanHandler.h"

VulkanHandler::VulkanHandler(WindowManager *windowManager) {
    this->windowManager = windowManager;
    this->windowExtent = windowManager->getWindowExtent();

    initVulkan();
}

void VulkanHandler::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    device.initVulkanDevice(instance, surface);
    createSwapChain();
    createRenderPass();
    createFramebuffers();
    createCommandPool();
}

void VulkanHandler::createInstance() {
    if (enableValidationLayers && !checkValidationLayersSupport(validationLayers)) {
        throw std::runtime_error("Unable to enable validation layers, not available.");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "bwqr";
    appInfo.apiVersion = VK_API_VERSION_1_1;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "bwqr game engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    auto requiredExtensions = getRequiredExtensions();

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.enabledExtensionCount = requiredExtensions.size();
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    VK_CHECK_RESULT(vkCreateInstance(&createInfo, nullptr, &instance))
}

void VulkanHandler::setupDebugMessenger() {
    if (enableValidationLayers) {
        VK_CHECK_RESULT(createDebugMessenger(instance, &debugMessenger))
    }
}

void VulkanHandler::createSurface() {
    VK_CHECK_RESULT(windowManager->createSurface(instance, &surface))
}

void VulkanHandler::updateFramebufferSize(VkExtent2D extent) {
    windowExtent = extent;
}

std::vector<const char *> VulkanHandler::getRequiredExtensions() {
    auto requiredExtensions = windowManager->getRequiredInstanceExtensions();

    if (enableValidationLayers) {
        requiredExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return requiredExtensions;
}

void VulkanHandler::createSwapChain() {
    swapChain.initSwapChain(&device, surface, windowExtent);
}

void VulkanHandler::createRenderPass() {
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChain.format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VK_CHECK_RESULT(vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &renderPass))
}

void VulkanHandler::createFramebuffers() {
    framebuffers.resize(swapChain.imageCount);

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.renderPass = renderPass;
    createInfo.attachmentCount = 1;
    createInfo.width = windowExtent.width;
    createInfo.height = windowExtent.height;
    createInfo.layers = 1;

    uint32_t i = 0;
    for (const auto &imageView: swapChain.imageViews) {

        createInfo.pAttachments = &imageView;

        VK_CHECK_RESULT(vkCreateFramebuffer(device.logicalDevice, &createInfo, nullptr, &framebuffers[i]))

        i++;
    }
}

void VulkanHandler::createCommandPool() {
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = device.queueFamilyIndices.graphicsFamily.value();

    VK_CHECK_RESULT(vkCreateCommandPool(device.logicalDevice, &createInfo, nullptr, &commandPool))
}

void VulkanHandler::resizeCleanup() {
    for (const auto &framebuffer: framebuffers) {
        vkDestroyFramebuffer(device.logicalDevice, framebuffer, nullptr);
    }

    vkDestroyRenderPass(device.logicalDevice, renderPass, nullptr);
}

void VulkanHandler::resizeCallback(VkExtent2D extent) {
    updateFramebufferSize(extent);
    swapChain.resizeCallback(extent);

    resizeCleanup();
    createRenderPass();
    createFramebuffers();
}