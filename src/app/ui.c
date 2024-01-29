#include "ui.h"
#include <core/window.h>

// centered floating window
void ui_win_float(int w, int h, rgbf bg_color, int title[], int title_len, font_t* font, rgbf text_col)
{
  // -- bg --
  int ww, wh;
  window_get_size(&ww, &wh);
  vec2 pos, size; 
  size[0] = w * 2;
  size[1] = h * 2;
  pos[0] =  ww - (size[0] / 2);
  pos[1] = -wh - (size[1] / 2);
  text_draw_quad(pos, size, bg_color);  

  // -- title --
  // pos[0] += w / 2;
  pos[1] -= font->gh * 1.5f;
  vec3_mul_f(bg_color, 0.85f, bg_color);
  text_draw_quad(pos, size, bg_color);  
  pos[1] += font->gh * 0.5f;
  text_draw_line_col(pos, title, title_len, text_col, font);
}

#define FILENODE_NAME_MAX 64
typedef struct filenode 
{
  int    name[FILENODE_NAME_MAX];
  int    name_len;
  struct filenode*  next;
  struct filenode** children;
  int    children_len;

}filenode;

void ui_make_filestruct(filenode* f, int f_max)
{
  filenode f0;
  // strcpy(f0.name, "test01.txt");
  f0.name[0] = 't'; f0.name[1] = 'e'; f0.name[2] = 's';
  f0.name[3] = 't'; f0.name[4] = '.'; f0.name[5] = 't';
  f0.name[6] = 'x'; f0.name[7] = 't';
  f0.name_len = 8;
  f0.children_len = 0;
  
  filenode f1;
  f1.name[0] = 't'; f1.name[1] = 'e'; f1.name[2] = 's';
  f1.name[3] = 't'; f1.name[4] = '.'; f1.name[5] = 't';
  f1.name[6] = 'x'; f1.name[7] = 't';
  f1.name_len = 8;
  f1.children_len = 0;
  f1.next = NULL;

  f[0] = f0;
  f[1] = f1;
  f[0].next = &f[1];
}

int win_files_title[] = { 'f', 'i', 'l', 'e', 's' };
int win_files_title_len = 5;
void ui_win_files(font_t* font)
{  
  int w, h;
  window_get_size(&w, &h);
  ui_win_float(w - 400, -h + 400, RGB_F_RGB(0.6f), 
      win_files_title, win_files_title_len, font, RGB_F_RGB(0.3f));
  
  filenode filestruct[2];
  ui_make_filestruct(filestruct, 2);

  vec2 pos = { (w - 400) * 2, (-h + 400) * 2 };
  // pos[1] -= font->gh * 2;
  for (int i = 0; i < 2; ++i)
  {
    text_draw_line(pos, filestruct[i].name, filestruct[i].name_len, font);
    pos[1] -= font->gh;
  }
}

