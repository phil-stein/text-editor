#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H

#include "text.h"
#include <math/math_inc.h>

void text_draw_init();
void text_draw_update();

void text_draw_glyph(vec2 pos, glyph* g);
void text_draw_glyph_box(vec2 pos, glyph* g, rgbf color);
void text_draw_img(vec2 pos, vec2 size, u32 tex, rgbf tint);
void text_draw_quad(vec2 pos, vec2 size, rgbf color);

void text_draw_line(vec2 pos, glyph** g, int g_len);
void text_line_pos(int _g, vec2 pos, glyph** g, int g_len);
void text_draw_screen(glyph** g, int g_len);
void text_screen_pos(int _g, vec2 pos, glyph** g, int g_len);

#endif
