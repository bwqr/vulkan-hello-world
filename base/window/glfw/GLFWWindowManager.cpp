#include "GLFWWindowManager.h"
GLFWWindowManager::GLFWWindowManager(int width, int height) {
    this->width = width;
    this->height = height;

    initWindow();
}

void GLFWWindowManager::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, "bwqr", nullptr, nullptr);
}

void GLFWWindowManager::setResizeCallback(void *application, void *callback) {
    glfwSetWindowUserPointer(window, application);

    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun) callback);
}

std::vector<const char *> GLFWWindowManager::getRequiredInstanceExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwRequiredInstanceExtensions;

    glfwRequiredInstanceExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwRequiredInstanceExtensions,
                                         glfwRequiredInstanceExtensions + glfwExtensionCount);

    return extensions;
}

VkExtent2D GLFWWindowManager::getWindowExtent() {
    int width, height;

    glfwGetWindowSize(window, &width, &height);

    return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

VkResult GLFWWindowManager::createSurface(VkInstance instance, VkSurfaceKHR *surfaceKhr) {
   return glfwCreateWindowSurface(instance, window, nullptr, surfaceKhr);
}

void GLFWWindowManager::waitEvents() {
    glfwWaitEvents();
}
