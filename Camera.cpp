#include "Camera.h"
#include "base/vulkan/VulkanHelper.h"

void Camera::updateUBO(size_t index) {
    glm::vec4 view4 = glm::mat4_cast(orientation) * glm::vec4(viewPlaneDirection, 1);
    glm::vec4 up4 = glm::mat4_cast(orientation) * glm::vec4(up, 1);
    glm::vec3 view3 = {view4.x, view4.y, view4.z};
    view = glm::lookAt(eye, eye + view3, {up4.x, up4.y, up4.z});
    proj = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 100.0f);
    proj[1][1] *= -1;
    ubo.viewProjection = proj * view;
    vbInfos[index].updateData(&ubo, sizeof(ubo));
}

void Camera::updateVBuffer(VulkanBuffer *uboVBuffer, VkDeviceSize offset, VkDeviceSize imageCount,
                           VkDeviceSize dynamicAlignment) {
    VirtualBufferInfo vbInfo = {};
    vbInfo.vBuffer = uboVBuffer;
    vbInfo.offset = offset;
    vbInfo.size = sizeof(Camera::ubo);

    vbInfos.reserve(imageCount);

    for (size_t i = 0; i < imageCount; i++) {
        vbInfos.push_back(vbInfo);
        vbInfo.offset += dynamicAlignment;
    }
}

void Camera::resizeCallback(VkExtent2D windowExtent) {
    aspect = windowExtent.width / static_cast<float>(windowExtent.height);
}

void Camera::rotateViewPlane(float dx, float dy) {
//    auto axis = glm::normalize(-viewPlaneDirection);
//
//    axis = axis * sinf(glm::radians(dx * 90.f));
//    auto scalar = cosf(glm::radians(dx * 90.f));
//
//    glm::fquat offset(scalar, axis.x, axis.y, axis.z);
//    orientation = orientation * offset;
    quaternionRotation({0, 0, 1}, glm::radians(-dx * 90.0f));
    quaternionRotation({1, 0, 0}, glm::radians(-dy * 90.0f));
//    viewPlaneDirection = glm::rotate(viewPlaneDirection, glm::radians(-dx * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    viewPlaneDirection = glm::rotate(viewPlaneDirection, glm::radians(-dy * 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void Camera::quaternionRotation(glm::vec3 axis, float rad) {
    axis = axis * sinf(rad);
    float scalar = cosf(rad);

    glm::fquat offset(scalar, axis.x, axis.y, axis.z);

    orientation = glm::normalize(orientation * offset);
}

void Camera::translateViewPlane(float dx, float dy) {
    auto view4 = glm::mat4_cast(orientation) * glm::vec4(viewPlaneDirection, 1);
    glm::vec3 view3 = {view4.x, view4.y, view4.z};

    eye += view3 * dy;
}
