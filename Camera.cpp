#include "Camera.h"
#include "base/vulkan/VulkanHelper.h"

void Camera::update(size_t index) {
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.0f, 10.0f);
    ubo.proj[1][1] *= -1;

    vbInfo.updateData(&ubo, sizeof(ubo) * index, sizeof(ubo));
}

VkDeviceSize Camera::updateVBuffer(VulkanBuffer *uboVBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                                   VkDeviceSize dynamicAlignment) {
    vbInfo.vBuffer = uboVBuffer;
    vbInfo.offset = offset;
    vbInfo.size = dynamicAlignment * imageCount;
    return vbInfo.size;
}

void Camera::resizeCallback(VkExtent2D windowExtent) {
    aspect = windowExtent.width / static_cast<float>(windowExtent.height);
}
