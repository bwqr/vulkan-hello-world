#include <zconf.h>
#include "Application.h"
#include "base/vulkan/VulkanShader.h"

Application::Application() : windowManager(WIDTH, HEIGHT) {

    //Dont use a stack based VulkanHandler, copy constructor is problematic
    vulkanHandler = new VulkanHandler(&windowManager);
    device = vulkanHandler->device.logicalDevice;

    windowManager.setResizeCallback(this, (void *) Application::resizeCallback);

    loadShaders();
    createGraphicsPipeline();
    createVertexBuffers();
    createCommandBuffers();
    createSyncPrimitives();
}

Application::~Application() {
    cleanup();
    delete vulkanHandler;
}

void Application::mainLoop() {
    while (!windowManager.shouldClose()) {
        windowManager.pollEvents();
        draw();
    }
}

void Application::createGraphicsPipeline() {

    VkPipelineShaderStageCreateInfo vertexStageCreateInfo = {};
    vertexStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexStageCreateInfo.pName = "main";
    vertexStageCreateInfo.module = vertShaderModule;

    VkPipelineShaderStageCreateInfo fragShaderCreateInfo = {};
    fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderCreateInfo.pName = "main";
    fragShaderCreateInfo.module = fragShaderModule;

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexStageCreateInfo, fragShaderCreateInfo};

    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(glm::vec3);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescription = {};
    attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription.binding = 0;
    attributeDescription.location = 0;
    attributeDescription.offset = 0;

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = &attributeDescription;

    VkPipelineInputAssemblyStateCreateInfo assemblyStateCreateInfo = {};
    assemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = vulkanHandler->windowExtent.width;
    viewport.height = vulkanHandler->windowExtent.height;
    viewport.maxDepth = 1.0f;
    viewport.minDepth = 0.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = vulkanHandler->windowExtent;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = &scissor;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = &viewport;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 0;

    VK_CHECK_RESULT(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout))

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineInfo.pInputAssemblyState = &assemblyStateCreateInfo;
    pipelineInfo.pViewportState = &viewportStateCreateInfo;
    pipelineInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = vulkanHandler->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline))
}

void Application::createCommandBuffers() {
    commandBuffers.resize(vulkanHandler->swapChain.imageCount);

    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandPool = vulkanHandler->commandPool;
    allocateInfo.commandBufferCount = commandBuffers.size();
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()))

    for (uint32_t i = 0; i < commandBuffers.size(); i++) {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffers[i], &beginInfo))

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = vulkanHandler->renderPass;
        renderPassBeginInfo.framebuffer = vulkanHandler->framebuffers[i];
        renderPassBeginInfo.renderArea.extent = vulkanHandler->windowExtent;
        renderPassBeginInfo.renderArea.offset = {0, 0};

        VkClearValue clearValue = {0.0f, 0.0f, 0.0f};
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, buffers, offsets);
        vkCmdDraw(commandBuffers[i], vertices.size(), 1, 0, 0);
        vkCmdEndRenderPass(commandBuffers[i]);

        VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[i]))
    }
}

void Application::createVertexBuffers() {
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = sizeof(glm::vec3) * vertices.size();
    createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK_RESULT(vkCreateBuffer(device, &createInfo, nullptr, &vertexBuffer))

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = vtr::findMemoryType(memRequirements.memoryTypeBits,
                                                    vulkanHandler->device.physicalDevice,
                                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &vertexBufferMemory))

    vkBindBufferMemory(device, vertexBuffer, vertexBufferMemory, 0);

    void *data;
    vkMapMemory(device, vertexBufferMemory, 0, createInfo.size, 0, &data);
    memcpy(data, vertices.data(), (size_t) createInfo.size);
    vkUnmapMemory(device, vertexBufferMemory);
}

void Application::draw() {
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, vulkanHandler->swapChain.swapChain,
                                            UINT64_MAX,
                                            imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        resizeApplication();
        std::cout << "out of date first " << currentFrame << std::endl;
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    if (vkQueueSubmit(vulkanHandler->device.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vulkanHandler->swapChain.swapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(vulkanHandler->device.presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;

        std::cout << "out of date second " << currentFrame << std::endl;

        resizeApplication();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Application::createSyncPrimitives() {
    imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    imagesInFlight.resize(vulkanHandler->swapChain.imageCount, VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        VK_CHECK_RESULT(
                vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                                  &imageAvailableSemaphores[i]))
        VK_CHECK_RESULT(
                vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr,
                                  &renderFinishedSemaphores[i]))
        VK_CHECK_RESULT(
                vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]))
    }
}

void Application::cleanup() {
    resizeCleanup();

    vkDestroyShaderModule(device, fragShaderModule, nullptr);
    vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void Application::resizeCleanup() {
    vkDeviceWaitIdle(device);
    vkFreeCommandBuffers(device, vulkanHandler->commandPool, commandBuffers.size(),
                         commandBuffers.data());
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

}

void Application::resizeApplication() {
    resizeCleanup();

    VkExtent2D extent = windowManager.getWindowExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = windowManager.getWindowExtent();
        windowManager.waitEvents();
    }

    vulkanHandler->resizeCallback(windowManager.getWindowExtent());

    createGraphicsPipeline();
    createCommandBuffers();
}

void Application::resizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

    app->framebufferResized = true;

    std::cout << "Resized" << std::endl;
}

void Application::loadShaders() {
    auto vertShaderCode = readFile("visual/shaders/vert.spv");
    auto fragShaderCode = readFile("visual/shaders/frag.spv");

    vertShaderModule = createShaderModule(device, vertShaderCode);
    fragShaderModule = createShaderModule(device, fragShaderCode);
}
