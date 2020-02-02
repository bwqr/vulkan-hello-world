#ifndef VULKAN_TRY_WINDOWMANAGER_H
#define VULKAN_TRY_WINDOWMANAGER_H


#include <vector>
#include <vulkan/vulkan.h>

class WindowManager {
public:
    virtual std::vector<const char *> getRequiredInstanceExtensions() = 0;

    virtual VkResult createSurface(VkInstance instance, VkSurfaceKHR *surfaceKhr) = 0;

    virtual void setResizeCallback(void *application, void *callback) = 0;

    virtual void pollEvents() = 0;

    virtual bool shouldClose() = 0;

    virtual VkExtent2D getWindowExtent() = 0;

    virtual void waitEvents() = 0;
};


#endif //VULKAN_TRY_WINDOWMANAGER_H
