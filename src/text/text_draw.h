#ifndef TEXT_DRAW_H
#define TEXT_DRAW_H

#include "text.h"
#include "text_codes.h"
#include <math/math_inc.h>

// static to avoid duplication when including header multiple times
// forced inline to always inline the functions
#define TEXT_D_INLINE static inline __attribute((always_inline))


void text_draw_init(font_t* font);
void text_draw_update();

void text_draw_glyph_col(vec2 pos, glyph* g, rgbf color);
TEXT_D_INLINE void text_draw_glyph(vec2 pos, glyph* g)
{ text_draw_glyph_col(pos, g, (float[3]){1, 1, 1}); }
void text_draw_glyph_box(vec2 pos, glyph* g, rgbf color);
void text_draw_img(vec2 pos, vec2 size, u32 tex, rgbf tint);
void text_draw_quad(vec2 pos, vec2 size, rgbf color);

void text_draw_line_col(vec2 pos, int* g, int g_len, rgbf color, font_t* font);
TEXT_D_INLINE void text_draw_line(vec2 pos, int* g, int g_len, font_t* font)
{ text_draw_line_col(pos, g, g_len, (float[3]){1, 1, 1}, font); }
void text_line_pos(int _g, vec2 pos, int* g, int g_len, font_t* font);
void text_draw_block(vec2 pos, int* g, int g_len, font_t* font);
void text_block_pos(int _g, vec2 pos, int* g, int g_len, font_t* font);

TEXT_D_INLINE int text_block_lines(int* g, int g_len)
{ 
  int l = 0; 
  for (int i = 0; i < g_len; ++i) 
  { if (g[i] == U_CR) { l++; } } 
  return l; 
}

#endif
