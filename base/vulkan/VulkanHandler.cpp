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

void VulkanHandler::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                                 VkBuffer &buffer, VkDeviceMemory &memory) {
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateBuffer(device.logicalDevice, &createInfo, nullptr, &buffer))

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device.logicalDevice, buffer, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, device.physicalDevice, properties);

    VK_CHECK_RESULT(vkAllocateMemory(device.logicalDevice, &allocateInfo, nullptr, &memory))

    vkBindBufferMemory(device.logicalDevice, buffer, memory, 0);
}

void VulkanHandler::copyBuffer(VkBuffer &srcBuffer, VkBuffer &dstBuffer, VkDeviceSize size) {
    auto commandBuffer = beginSingleTimeCommands();

    VkBufferCopy bufferCopy = {};
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

    endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VulkanHandler::beginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = commandPool;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device.logicalDevice, &allocateInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void VulkanHandler::endSingleTimeCommands(VkCommandBuffer &commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkDeviceWaitIdle(device.logicalDevice);

    vkFreeCommandBuffers(device.logicalDevice, commandPool, 1, &commandBuffer);
}

void
VulkanHandler::createModelsBuffer(VkBuffer &modelBuffer, VkDeviceMemory &modelBufferMemory,
                                  const std::vector<VulkanModel> &models, VkDeviceSize *indexOffset) {
    VkDeviceSize vertexBufferSize = 0;
    VkDeviceSize indexBufferSize = 0;
    for (const auto &model: models) {
        vertexBufferSize += model.vertexBufferSize;
        indexBufferSize += model.indexBufferSize;
    }

    VkDeviceSize size = vertexBufferSize + indexBufferSize;
    *indexOffset = vertexBufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    createBuffer(size,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingMemory);

    void *mem;

    vkMapMemory(device.logicalDevice, stagingMemory, 0, size, 0, &mem);

    for (const auto &model: models) {
        memcpy(mem, model.vertices.data(), (size_t) model.vertexBufferSize);
        mem = static_cast<char *>(mem) + model.vertexBufferSize;
    }

    for (const auto &model: models) {
        memcpy(mem, model.indices.data(), (size_t) model.indexBufferSize);
        mem = static_cast<char *>(mem) + model.indexBufferSize;
    }

    vkUnmapMemory(device.logicalDevice, stagingMemory);

    createBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 modelBuffer, modelBufferMemory);

    copyBuffer(stagingBuffer, modelBuffer, size);

    vkDestroyBuffer(device.logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(device.logicalDevice, stagingMemory, nullptr);
}

