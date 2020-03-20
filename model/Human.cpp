#include "Human.h"
#include "../base/vulkan/VulkanHelper.h"

Human::Human(VertexSet *vs) : Model(vs) {}

void Human::update(size_t index) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    static float k = .2;

    ubo.model = glm::rotate(glm::mat4(1.0f), k * time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    vbInfo.updateData(&ubo.model, sizeof(ubo) * index, sizeof(ubo));
}
