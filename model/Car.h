

#ifndef VULKAN_HELLO_WORLD_CAR_H
#define VULKAN_HELLO_WORLD_CAR_H


#include "Model.h"

class Car : public Model {
public:
    Car(VertexSet *vs, glm::int32 texIndex);

    void update(size_t index) override;
};


#endif //VULKAN_HELLO_WORLD_CAR_H
