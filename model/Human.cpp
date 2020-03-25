#include "Human.h"
#include "../base/vulkan/VulkanHelper.h"

Human::Human(VertexSet *vs, glm::int32 texIndex) : Model(vs, texIndex) {}

void Human::updateUBO(size_t index) {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    auto translateMat = glm::translate(glm::mat4(1.0f), position);
    auto rotationMat = glm::rotate(translateMat, speed * time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    auto scaleMat = glm::mat4(1.0f);
    scaleMat[0][0] = scale.x;
    scaleMat[1][1] = scale.y;
    scaleMat[2][2] = scale.z;
    scaleMat[3][3] = 1;

    ubo.model = rotationMat * scaleMat;

    vbInfos[index].updateData(&ubo.model, sizeof(ubo));
}
