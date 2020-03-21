#include "Car.h"
#include "../base/vulkan/VulkanHelper.h"

Car::Car(VertexSet *vs) : Model(vs) {}

void Car::update(size_t index) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    ubo.model = glm::rotate(glm::mat4(1.0f), 2 * time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    vbInfos[index].updateData(&ubo.model,sizeof(ubo));
}
