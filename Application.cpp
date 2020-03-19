#include "Application.h"
#include "base/vulkan/VulkanShader.h"
#include "Model.h"
#include "model/Car.h"
#include "model/Human.h"

Application::Application() : windowManager(WIDTH, HEIGHT) {

    //Dont use a stack based VulkanHandler, copy constructor is problematic
    vulkanHandler = new VulkanHandler(&windowManager);
    device = vulkanHandler->device.logicalDevice;

    windowManager.setResizeCallback(this, (void *) Application::resizeCallback);

    loadModels();
    loadShaders();
    createUboBuffer();
    createDescriptorPool();
    createDescriptorSetLayout();
    createDescriptorSets();
    createGraphicsPipeline();
//    createOverlay();
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

        static int fps = 0;

        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();

        if (std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count() > 1) {
            std::cout << fps << std::endl;
            startTime = currentTime;
            fps = 0;
        }

        fps++;

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

    auto attributeDescription = Vertex::attributeDescription();
    auto bindingDescription = Vertex::bindingDescription();

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescription.size();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescription.data();

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
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

    VkDescriptorSetLayout layouts[2] = {cameraDescriptorSetLayout, modelDescriptorSetLayout};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &modelDescriptorSetLayout;
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
    pipelineInfo.pDepthStencilState = &depthStencil;
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

        VkBuffer buffers[] = {vertexSetVbuffer.buffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffers[i], vertexSetVbuffer.buffer, indexOffset, VK_INDEX_TYPE_UINT32);

        uint32_t j = 0;
        uint32_t modelIndexOffset = 0;
        uint32_t modelVertexOffset = 0;
        uint32_t dynamicOffset;
        for (auto &model: models) {
            dynamicOffset = 2 * dynamicAlignment + (vulkanHandler->swapChain.imageCount * j + i) * static_cast<uint32_t>(dynamicAlignment);

            vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
                                    1, &model->getDescriptorSet(i), 1, &dynamicOffset);

            vkCmdDrawIndexed(commandBuffers[i], model->vertexSet->indices.size(), 1,
                             modelIndexOffset,
                             modelVertexOffset, 0);
            modelVertexOffset = model->vertexSet->vertices.size();
            modelIndexOffset = model->vertexSet->indices.size();
            j++;
        }
        vkCmdEndRenderPass(commandBuffers[i]);

        VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffers[i]))
    }
}

void Application::loadModels() {
    VertexSet vs = {};
    vs.vertices = {{{.5, .0, .0}, {.0,  .0,  1.0}},
                   {{.0, .0, .0}, {1.0, 1.0, 1.0}},
                   {{.0, .5, .0}, {1.0, .0,  .0}},
                   {{.5, .5, .0}, {.0,  1.0, .0}}};
    vs.indices = {{0, 2, 1, 0, 3, 2}};
    vertexSets.push_back(vs);

    vs.vertices = {
            {{.0,  .0,  .0}, {.0,  .0,  1.0}},
            {{-.5, -.5, .0}, {1.0, .0,  .0}},
            {{-.5, .0,  .0}, {.0,  1.0, .0}},
    };
    vs.indices = {{0, 2, 1}};
    vertexSets.push_back(vs);

    vulkanHandler->
            createVertexSetsBuffer(vertexSetVbuffer, vertexSets, &indexOffset
    );

    models.emplace_back(new
                                Car(&vertexSets[0])
    );

    models.emplace_back(new
                                Human(&vertexSets[1])
    );
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
        return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }

    imagesInFlight[imageIndex] = inFlightFences[currentFrame];

    updateUniformBuffers(imageIndex);

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

    VK_CHECK_RESULT(vkQueueSubmit(vulkanHandler->device.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]))

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

    vkDestroyBuffer(device, vertexSetVbuffer.buffer, nullptr);
    vkFreeMemory(device, vertexSetVbuffer.memory, nullptr);

    vkDestroyDescriptorSetLayout(device, cameraDescriptorSetLayout, nullptr);
}

void Application::resizeCleanup() {
    vkDeviceWaitIdle(device);
    vkFreeCommandBuffers(device, vulkanHandler->commandPool, commandBuffers.size(),
                         commandBuffers.data());
    vkDestroyPipeline(device, graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

    vkDestroyBuffer(device, uboVBuffer.buffer, nullptr);
    vkFreeMemory(device, uboVBuffer.memory, nullptr);

    vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void Application::resizeApplication() {
    resizeCleanup();

    VkExtent2D extent = windowManager.getWindowExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = windowManager.getWindowExtent();
        windowManager.waitEvents();
    }

    windowExtent = extent;

    vulkanHandler->resizeCallback(extent);

    createUboBuffer();
    createDescriptorPool();
    createDescriptorSets();
    createGraphicsPipeline();
    createCommandBuffers();
}

void Application::loadShaders() {
    auto vertShaderCode = readFile("visual/shaders/vert.spv");
    auto fragShaderCode = readFile("visual/shaders/frag.spv");

    vertShaderModule = createShaderModule(device, vertShaderCode);
    fragShaderModule = createShaderModule(device, fragShaderCode);
}

void Application::updateUniformBuffers(uint32_t index) {
    uboVBuffer.map(0, VK_WHOLE_SIZE);
    for (auto &model: models) {
        model->update(index);
    }
    uboVBuffer.unmap();
}

void Application::createUboBuffer() {
    VkDeviceSize minBufferAlignment = vulkanHandler->device.properties.limits.minUniformBufferOffsetAlignment;
    dynamicAlignment = sizeof(Model::ubo);

    if (minBufferAlignment > 0) {
        dynamicAlignment = (dynamicAlignment + minBufferAlignment - 1) & ~(minBufferAlignment - 1);
    }

    VkDeviceSize bufferSize = models.size() * vulkanHandler->swapChain.imageCount * dynamicAlignment +
                              2 * dynamicAlignment; //For camera ubo

    vulkanHandler->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                uboVBuffer);


    VkDeviceSize offset = dynamicAlignment * 2;
    for (auto &model: models) {
        offset += model->updateVBuffer(&uboVBuffer, offset, vulkanHandler->swapChain.imageCount, dynamicAlignment);
    }
}

void Application::createDescriptorSets() {
    uint32_t imageCount = vulkanHandler->swapChain.imageCount;

    for (auto &model: models) {
        VK_CHECK_RESULT(model->createDescriptorSets(descriptorPool, modelDescriptorSetLayout, imageCount,
                                                    VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC))
    }
}

void Application::createDescriptorSetLayout() {
    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    createInfo.bindingCount = 1;
    createInfo.pBindings = &uboLayoutBinding;

//    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &cameraDescriptorSetLayout))

    uboLayoutBinding.binding = 0;

    VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &modelDescriptorSetLayout))
}

void Application::createDescriptorPool() {
    VkDescriptorPoolSize poolSize = {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSize.descriptorCount = vulkanHandler->swapChain.imageCount * models.size();

    VkDescriptorPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.poolSizeCount = 1;
    createInfo.pPoolSizes = &poolSize;
    createInfo.maxSets = vulkanHandler->swapChain.imageCount * models.size();

    VK_CHECK_RESULT(vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool))
}

void Application::resizeCallback(GLFWwindow *window, int width, int height) {
    auto app = reinterpret_cast<Application *>(glfwGetWindowUserPointer(window));

    app->framebufferResized = true;

    std::cout << "Resized" << std::endl;
}