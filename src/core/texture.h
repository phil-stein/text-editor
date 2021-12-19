#ifndef TEXTURE_H
#define TEXTURE_H

#include "core.h"


typedef struct texture
{
	u32 handle;
	int width;
	int height;
	char* name;
	char* path;
}texture;


void texture_load_pixels(const char* path, u8** pixels_out, size_t* width_out, size_t* height_out, int* channel_num, bool flip_vertical);

u32 texture_create_from_pixels(u8* pixels, size_t width, size_t height, int channel_num);

texture texture_create_from_path(const char* file_path, const char* name, bool flip_vertical);


#endif
