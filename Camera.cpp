#include "Camera.h"
#include "base/vulkan/VulkanHelper.h"

void Camera::updateUBO(size_t index) {
    view = glm::lookAt(eye, eye + oViewPlaneVector, orientedUpVector);
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
    static glm::fquat baseOrientation = {1.0f, 0.0f, 0.0f, 0.0f};

    static float sensitivity = 45.0f;
    static float div = 90.0f / sensitivity / 2.0f;
    float fracpart, intpart;
    fracpart = std::modf(yaw + dx, &intpart);
    yaw = static_cast<int>(intpart) % static_cast<int>(360.0f / 2.0f) + fracpart;
    pitch = std::max(std::min(pitch + dy, div), -div);

    xOrientation = glm::mat3_cast(
            quaternionRotation(baseOrientation, {0, 0, 1}, glm::radians(-yaw * sensitivity)));
    zOrientation = glm::mat3_cast(
            quaternionRotation(baseOrientation, {1, 0, 0}, glm::radians(-pitch * sensitivity)));

    oViewPlaneVector = xOrientation * zOrientation * viewPlaneVector;

    orientedUpVector = xOrientation * zOrientation * upVector;
}

void Camera::translateViewPlane(float dx, float dy) {
    eye += xOrientation * glm::vec3(1, 0, 0) * dx + xOrientation * glm::vec3(0, 1, 0) * dy;
}

glm::fquat Camera::quaternionRotation(const glm::fquat &quat, glm::vec3 axis, float rad) {
    axis = axis * sinf(rad);
    float scalar = cosf(rad);

    glm::fquat offset(scalar, axis.x, axis.y, axis.z);

    return quat * offset;
}
