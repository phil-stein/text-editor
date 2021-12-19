#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H

#include "text.h"
#include "../math/math_inc.h"

void text_draw_init();
void text_draw_update();

void text_draw_glyph(vec2 pos, vec2 size, glyph_info* g);
void text_draw_glyph_box(vec2 pos, vec2 size, glyph_info* g, vec3 color);
void text_draw_screen(glyph_info* g, int g_len);


#endif
