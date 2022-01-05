#include "texture.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#define STB_IMAGE_IMPLEMENTATION	// only define once
#include <stb/stb_image.h>
#pragma GCC diagnostic pop
#include <GLAD/glad.h>

void texture_load_pixels(const char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bool flip_vertical) 
{
    int width, height;

    // OpenGL has texture coordinates with (0, 0) on bottom
    stbi_set_flip_vertically_on_load(flip_vertical);
    u8* image = stbi_load(path, &width, &height, channel_num, STBI_rgb_alpha);
    assert(image != NULL);

    *pixels_out = malloc((double)(width * height * 4));
    assert(*pixels_out != NULL);
    memcpy(*pixels_out, image, (double)(width * height * 4));
    assert(*pixels_out != NULL);
    *width_out = width;
    *height_out = height;

    stbi_image_free(image);
}

u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num)
{
    u32 handle;

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    // no interpolation
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    assert(channel_num >= 1);
    assert(channel_num != 2);
    assert(channel_num <= 4);
    int gl_internal_format = 0;
    int gl_format          = 0;
    switch (channel_num)
    {
        case 1:
            gl_internal_format = GL_R8;
            gl_format = GL_RED;
        case 3:
            gl_internal_format = GL_RGB;
            gl_format = GL_RGB;
        case 4:
            gl_internal_format = GL_RGBA; // GL_SRGB_APLHA
            gl_format = GL_RGBA;
    }
    assert(gl_format != 0);

    glTexImage2D(GL_TEXTURE_2D, 0, gl_internal_format, width, height, 0, gl_format, GL_UNSIGNED_BYTE, pixels);
    return handle;
}

texture texture_create_from_path(const char* file_path, const char* name, bool flip_vertical)
{
    u8* pixels;
    size_t width, height;
    int channel_num = 0;
    texture_load_pixels(file_path, &pixels, &width, &height, &channel_num, flip_vertical);
    u32 handle = texture_create_from_pixels(pixels, width, height, channel_num);
    free(pixels);

    texture tex;
    tex.handle = handle;
    tex.width = width;
    tex.height = height;
    tex.name = (char*)name;
    tex.path = (char*)file_path;

    // printf("loaded texture: '%s' from '%s', handle: '%d'\n", name, file_path, handle);

    return tex;
}

