#ifndef UI_H
#define UI_H

#include <global/global.h>
#include <text/text_inc.h>
#include <math/math_inc.h>

void ui_win_float(int w, int h, rgbf bg_color, int title[], int title_len, font_t* font, rgbf text_col);
void ui_win_files(font_t* font);

#endif
