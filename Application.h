#ifndef VULKAN_TRY_APPLICATION_H
#define VULKAN_TRY_APPLICATION_H


#include "base/vulkan/VulkanHandler.h"
#include "base/window/glfw/GLFWWindowManager.h"
#include "base/vulkan/VulkanModel.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "thirdparty/imgui/imgui.h"
#include "thirdparty/imgui/examples/imgui_impl_glfw.h"
#include "thirdparty/imgui/examples/imgui_impl_vulkan.h"

#include <chrono>

#define WIDTH 800
#define HEIGHT 600

class Application {
public:
    Application();

    ~Application();

    static void resizeCallback(GLFWwindow *window, int width, int height);

    void mainLoop();

private:
    bool framebufferResized = false;

    VkDevice device;

    std::vector<VulkanModel> models;

    VulkanHandler *vulkanHandler = nullptr;
    GLFWWindowManager windowManager;
    VkExtent2D windowExtent;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;

    VkPipeline graphicsPipeline;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    VkBuffer modelBuffer;
    VkDeviceMemory modelBufferMemory;
    VkBuffer uboBuffer;
    VkDeviceMemory uboBufferMemory;

    VkDeviceSize dynamicAlignment;
    VkDeviceSize indexOffset;

    std::vector<VkCommandBuffer> commandBuffers;

    size_t currentFrame = 0;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    void createGraphicsPipeline();

    void createCommandBuffers();

    void loadModels();

    void createSyncPrimitives();

    void createOverlay();

    void resizeApplication();

    void draw();

    void cleanup();

    void resizeCleanup();

    void loadShaders();

    void createDescriptorSetLayout();

    void updateUniformBuffers(uint32_t index);

    void createUboBuffer();

    void createDescriptorSets();

    void createDescriptorPool();
};


#endif //VULKAN_TRY_APPLICATION_H
