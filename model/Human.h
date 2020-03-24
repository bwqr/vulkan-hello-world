

#ifndef VULKAN_HELLO_WORLD_HUMAN_H
#define VULKAN_HELLO_WORLD_HUMAN_H


#include "Model.h"

class Human : public Model {
public:
    Human(VertexSet *vs, glm::int32 texIndex);

    void update(size_t index) override;
};


#endif //VULKAN_HELLO_WORLD_HUMAN_H
