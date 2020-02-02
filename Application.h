#ifndef VULKAN_TRY_APPLICATION_H
#define VULKAN_TRY_APPLICATION_H


#include "base/vulkan/VulkanHandler.h"
#include "base/window/glfw/GLFWWindowManager.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define WIDTH 800
#define HEIGHT 600

static std::vector<glm::vec3> vertices = {
        {.5, .0, .0},
        {.0, .5, .0},
        {.5, .5, .0},
        {.0, .5, .0},
        {.5, .0, .0},
        {.5, .25, .0}
};

class Application {
public:
    Application();

    ~Application();

    static void resizeCallback(GLFWwindow *window, int width, int height);

    void mainLoop();

private:
    bool framebufferResized = false;

    VkDevice device;

    VulkanHandler *vulkanHandler = nullptr;
    GLFWWindowManager windowManager;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;

    std::vector<VkCommandBuffer> commandBuffers;

    size_t currentFrame = 0;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;

    void createGraphicsPipeline();

    void createCommandBuffers();

    void createVertexBuffers();

    void createSyncPrimitives();

    void resizeApplication();

    void draw();

    void cleanup();

    void resizeCleanup();

    void loadShaders();
};


#endif //VULKAN_TRY_APPLICATION_H
