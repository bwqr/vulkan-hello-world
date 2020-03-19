

#ifndef VULKAN_HELLO_WORLD_HUMAN_H
#define VULKAN_HELLO_WORLD_HUMAN_H


#include "../Model.h"

class Human : public Model {
public:
    Human(VertexSet *vs);

    void update(size_t index) override;

    VkResult createDescriptorSets(const VkDescriptorPool &pool, const VkDescriptorSetLayout &setLayout,
                                  VkDeviceSize imageCount, VkDescriptorType descriptorType) override;
};


#endif //VULKAN_HELLO_WORLD_HUMAN_H
