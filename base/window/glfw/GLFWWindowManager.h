#ifndef VULKAN_TRY_GLFWWINDOWMANAGER_H
#define VULKAN_TRY_GLFWWINDOWMANAGER_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "../WindowManager.h"

class GLFWWindowManager : public WindowManager {
public:
    GLFWwindow *window;

    GLFWWindowManager(int width, int height);

    ~GLFWWindowManager() {
        glfwDestroyWindow(window);
        glfwTerminate();
    };

    VkResult createSurface(VkInstance instance, VkSurfaceKHR *surfaceKhr) override;

    inline bool shouldClose() override {
        return glfwWindowShouldClose(window);
    };

    inline void pollEvents() override {
        glfwPollEvents();
    };

    void setResizeCallback(void *application, void *callback) override;

    void setKeyCallback(void *application, void *callback) override;

    void setCursorPosCallback(void *application, void *callback) override;

    void getCursorPos(double *xpos, double *ypos) override;

    std::vector<const char *> getRequiredInstanceExtensions() override;

    VkExtent2D getWindowExtent() override;

    void waitEvents() override;

private:
    int width;
    int height;

    void initWindow();
};

#endif //VULKAN_TRY_GLFWWINDOWMANAGER_H
