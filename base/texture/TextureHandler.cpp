#define STB_IMAGE_IMPLEMENTATION
#include "TextureHandler.h"

stbi_uc *TextureHandler::loadTexture(const char *path, int *width, int *height, int *channels) {
    stbi_uc *pixels = stbi_load(path, width, height, channels, STBI_rgb_alpha);

    return pixels;
}

void TextureHandler::unloadTexture(stbi_uc *pixels) {
    stbi_image_free(pixels);
}
