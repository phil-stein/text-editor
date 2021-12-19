#include "app.h"
#include "../core/input.h"
#include "../core/renderer.h"
#include "../core/time.h"
#include "../core/file_io.h"
#include "../text/text_inc.h"

#include <ctype.h>

bool wiref_act = false;

#define BUFFER_MAX 256
glyph_info buffer[BUFFER_MAX];
int        buffer_pos = 0;

void app_init()
{
  // buffer[buffer_pos++] = text_get_info(0x000000e4); // ä
  // buffer[buffer_pos++] = text_get_info(0x000000DF); // ß
  // buffer[buffer_pos++] = text_get_info(0x000000e6); // ae
  // // buffer[buffer_pos++] = text_get_info(0x000003fc); // rho
  // // buffer[buffer_pos++] = text_get_info(0xcfbc); // rho
  // // buffer[buffer_pos++] = text_get_info(0x0001F600); // :)
  // // buffer[buffer_pos++] = text_get_info(0x00002016); // ||
  // buffer[buffer_pos++] = text_get_info(0x00002021); // cross
  // // buffer[buffer_pos++] = text_get_info(0x00002023); // arrow
  // // buffer[buffer_pos++] = text_get_info(0x0000202B); // dotted cross
  // buffer[buffer_pos++] = text_get_info(0x0000000D); // \n

  // for (int i = 32; i < 127; ++i)
  // { buffer[buffer_pos++] = text_get_info(i); }
  
  int txt_len = 0;
  char* txt = read_text_file_len("assets/examples/uga.buga", &txt_len);
  P_STR(txt);
  for (int i = 0; i < txt_len; ++i)
  { 
    if (txt[i] == ' ')  
    { buffer[buffer_pos++] = text_get_info(U_SPACE); }
    if (txt[i] == '\n') 
    { buffer[buffer_pos++] = text_get_info(U_CR); }
    if (isspace(txt[i]))
    { continue; }
    buffer[buffer_pos++] = text_get_info(txt[i]); 
  }
  
}

void app_update()
{
  text_draw_screen(buffer, buffer_pos);  
	
  if (is_key_pressed(KEY_Escape))
  {
    app_quit();  
  }
  if (is_key_pressed(KEY_Tab))
	{
		wiref_act = !wiref_act;
		renderer_set_wireframe(wiref_act);
	}

	f32 dist = 4.0f * get_delta_t();
	if (is_key_down(KEY_LeftArrow))
	{
		renderer_move_cam(VEC3_X(-dist));
	}
	if (is_key_down(KEY_RightArrow))
	{
		renderer_move_cam(VEC3_X(dist));
	}
	if (is_key_down(KEY_UpArrow))
	{
		renderer_move_cam(VEC3_Y(-dist));
	}
	if (is_key_down(KEY_DownArrow))
	{
		renderer_move_cam(VEC3_Y(dist));
	}

}



