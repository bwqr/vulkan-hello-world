#ifndef VULKAN_HELLO_WORLD_TEXTUREHANDLER_H
#define VULKAN_HELLO_WORLD_TEXTUREHANDLER_H

#include <string>
#include "../../thirdparty/stb/stb_image.h"

class TextureHandler {
public:
    static stbi_uc *loadTexture(std::string path, int *width, int *height, int *channels);

    static void unloadTexture(stbi_uc *pixels);
};


#endif //VULKAN_HELLO_WORLD_TEXTUREHANDLER_H
