#include "Camera.h"
#include "base/vulkan/VulkanHelper.h"

void Camera::update(size_t index) {
    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), aspect, 0.0f, 10.0f);
    ubo.proj[1][1] *= -1;

    vbInfos[index].updateData(&ubo, sizeof(ubo));
}

void Camera::updateVBuffer(VulkanBuffer *uboVBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                           VkDeviceSize dynamicAlignment) {
    VirtualBufferInfo vbInfo = {};
    vbInfo.vBuffer = uboVBuffer;
    vbInfo.offset = offset;
    vbInfo.size = dynamicAlignment * 2;

    vbInfos.reserve(imageCount);

    for (size_t i = 0; i < imageCount; i++) {
        vbInfos.push_back(vbInfo);
        vbInfo.offset += vbInfo.size;
    }
}

void Camera::resizeCallback(VkExtent2D windowExtent) {
    aspect = windowExtent.width / static_cast<float>(windowExtent.height);
}
