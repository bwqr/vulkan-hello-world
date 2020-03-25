#ifndef VULKAN_TRY_APPLICATION_H
#define VULKAN_TRY_APPLICATION_H


#include "base/vulkan/VulkanHandler.h"
#include "base/window/glfw/GLFWWindowManager.h"
#include "base/vulkan/VulkanBuffer.h"
#include "VertexSet.h"
#include "model/Model.h"
#include "Camera.h"
#include "base/vulkan/VulkanImage.h"
#include "base/texture/TextureHandler.h"
#include "base/vulkan/VulkanShader.h"
#include "model/Model.h"
#include "model/Human.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <memory>

#define WIDTH 800
#define HEIGHT 600

class Application {
public:
    Application();

    ~Application();

    std::chrono::high_resolution_clock::time_point lastFrameTime;

    void mainLoop();

private:
    bool framebufferResized = false;

    VkDevice device;

    std::vector<std::unique_ptr<Model>> models;
    Camera camera;

    VulkanHandler *vulkanHandler = nullptr;
    GLFWWindowManager windowManager;
    VkExtent2D windowExtent;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    VkPipelineLayout pipelineLayout;

    VkPipeline graphicsPipeline;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    std::vector<VertexSet> vertexSets;
    VulkanBuffer vertexSetVBuffer;
    VulkanBuffer uboVBuffer;
    std::vector<VulkanImage> vTextures;

    VkDeviceSize modelDynamicAlignment;
    VkDeviceSize cameraDynamicAlignment;
    VkDeviceSize indexOffset;

    std::vector<VkCommandBuffer> commandBuffers;

    size_t currentFrame = 0;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    struct {
        double xpos;
        double ypos;
        double dx;
        double dy;
    } cursor;

    bool cameraZRotation = false;
    bool cameraXPlusMove = false;
    bool cameraXNegMove = false;
    bool cameraYPlusMove = false;
    bool cameraYNegMove = false;
    void createGraphicsPipeline();

    void createCommandBuffers();

    void loadModels();

    void createSyncPrimitives();

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

    void loadTextures();

    static void resizeCallback(GLFWwindow *window, int width, int height);

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    void updateCamera(bool controller);
};


#endif //VULKAN_TRY_APPLICATION_H
