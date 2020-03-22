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
                                 VulkanBuffer &vBuffer) {
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateBuffer(device.logicalDevice, &createInfo, nullptr, &vBuffer.buffer))

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device.logicalDevice, vBuffer.buffer, &memoryRequirements);

    vBuffer.size = memoryRequirements.size;
    vBuffer.alignment = memoryRequirements.alignment;
    vBuffer.device = device.logicalDevice;

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = findMemoryType(memoryRequirements.memoryTypeBits, device.memoryProperties,
                                                  properties);

    VK_CHECK_RESULT(vkAllocateMemory(device.logicalDevice, &allocateInfo, nullptr, &vBuffer.memory))

    vkBindBufferMemory(device.logicalDevice, vBuffer.buffer, vBuffer.memory, 0);
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
VulkanHandler::createVertexSetsBuffer(VulkanBuffer &vBuffer,
                                      std::vector<VertexSet> &vertexSets, VkDeviceSize *indexOffset) {
    VkDeviceSize bufferSize = 0;
    for (auto &vs: vertexSets) {
        vs.updateVertexVBInfo(bufferSize);
        bufferSize += vs.vertexVBInfo.size;
    }
    *indexOffset = bufferSize;

    size_t offset = 0;
    for (auto &vs: vertexSets) {
        vs.updateIndexVBInfo(offset);
        offset += vs.indexVBInfo.size;
        bufferSize += vs.indexVBInfo.size;
    }

    VulkanBuffer stagingVBuffer;

    createBuffer(bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingVBuffer);

    stagingVBuffer.map(0, stagingVBuffer.size);
    for (const auto &vs: vertexSets) {
        stagingVBuffer.copy(vs.vertices.data(), vs.vertexVBInfo.offset, vs.vertexVBInfo.size);
    }

    for (const auto &vs: vertexSets) {
        stagingVBuffer.copy(vs.indices.data(), *indexOffset + vs.indexVBInfo.offset, vs.indexVBInfo.size);
    }

    stagingVBuffer.unmap();

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 vBuffer);

    copyBuffer(stagingVBuffer.buffer, vBuffer.buffer, bufferSize);

    vkDestroyBuffer(device.logicalDevice, stagingVBuffer.buffer, nullptr);
    vkFreeMemory(device.logicalDevice, stagingVBuffer.memory, nullptr);
}

void VulkanHandler::createTexture(VkDeviceSize imageSize, void *data, uint32_t width, uint32_t height,
                                  VulkanTexture *vTexture) {
    VulkanBuffer stagingVBuffer;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingVBuffer);

    void *map;
    vkMapMemory(device.logicalDevice, stagingVBuffer.memory, 0, imageSize, 0, &map);
    memcpy(map, data, static_cast<size_t>(imageSize));
    vkUnmapMemory(device.logicalDevice, stagingVBuffer.memory);

    createImage(width, height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                vTexture);

    transitionImageLayout(vTexture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(stagingVBuffer.buffer, vTexture->image, static_cast<uint32_t>(width),
                      static_cast<uint32_t>(height));
    transitionImageLayout(vTexture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device.logicalDevice, stagingVBuffer.buffer, nullptr);
    vkFreeMemory(device.logicalDevice, stagingVBuffer.memory, nullptr);

    swapChain.createImageView(vTexture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1,
                              &vTexture->imageView);

    createTextureSampler(&vTexture->sampler);

}

void VulkanHandler::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                                VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VulkanTexture *vTexture) {
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateImage(device.logicalDevice, &imageInfo, nullptr, &vTexture->image))

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device.logicalDevice, vTexture->image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, device.memoryProperties, properties);

    VK_CHECK_RESULT(vkAllocateMemory(device.logicalDevice, &allocInfo, nullptr, &vTexture->memory))

    vkBindImageMemory(device.logicalDevice, vTexture->image, vTexture->memory, 0);
}

void VulkanHandler::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout,
                                          VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
            commandBuffer,
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void VulkanHandler::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
            width,
            height,
            1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void VulkanHandler::createTextureSampler(VkSampler *sampler) {
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    VK_CHECK_RESULT(vkCreateSampler(device.logicalDevice, &samplerInfo, nullptr, sampler))
}

