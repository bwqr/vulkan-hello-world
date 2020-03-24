#include "Human.h"
#include "../base/vulkan/VulkanHelper.h"

Human::Human(VertexSet *vs, glm::int32 texIndex) : Model(vs, texIndex) {}

void Human::update(size_t index) {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    auto positionMat = glm::translate(glm::mat4(scale), position);
    positionMat[3][3] = 1;

    ubo.model = glm::rotate(positionMat, speed * time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    vbInfos[index].updateData(&ubo.model, sizeof(ubo));
}
