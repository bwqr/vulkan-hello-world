#include "Camera.h"
#include "base/vulkan/VulkanHelper.h"

void Camera::update(size_t index) {
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), windowExtent.width / (float) windowExtent.height, 0.0f, 10.0f);
    ubo.proj[1][1] *= -1;

    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

//    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));

    vbInfo.updateData(&ubo, sizeof(ubo) * index, sizeof(ubo));
}

VkDeviceSize Camera::updateVBuffer(VulkanBuffer *uboVBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                                   VkDeviceSize dynamicAlignment) {
    vbInfo.vBuffer = uboVBuffer;
    vbInfo.offset = offset;
    vbInfo.size = dynamicAlignment * imageCount;
    return vbInfo.size;
}
