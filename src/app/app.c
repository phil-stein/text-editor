#include "app.h"
#include "cmd.h"
#include <core/input.h>
#include <core/renderer.h>
#include <core/time.h>
#include <core/file_io.h>
#include <core/window.h>
#include <text/text_inc.h>
#include <math/math_inc.h>

#include <ctype.h>
#include <direct.h>


bool wiref_act = false;

#define CWD_MAX 128
char cwd[CWD_MAX];

#define BUFFER_MAX 512
glyph* buffer[BUFFER_MAX];
u32    buffer_pos = 0;

int cursor = 0;

#define CMD_KEY   '>'
#define CMD_START 0x25B6 // '>'/  / 0x2192 
#define CMD_MAX   128
glyph* cmd[CMD_MAX];
int    cmd_len = 0;
bool   in_cmd = false; 

#define OUT_MAX 64
glyph* out[OUT_MAX];
int    out_pos = 0;


#define CMD_PSH(c)      if (cmd_len < CMD_MAX) { cmd[cmd_len++] = text_get_glyph(c); }
#define CMD_POP()       cmd_len = cmd_len -1 >= 1 ? cmd_len -1 : 1 
#define CMD_FLUSH()     cmd_len = 1
#define CMD_TRUNC(len)  cmd_len = cmd_len - len >= 1 ? cmd_len - len : 1

#define OUT_FILL(str)   for (out_pos = 0; out_pos < strlen(str) && out_pos < OUT_MAX; ++out_pos)  \
                        { out[out_pos] = text_get_glyph(str[out_pos]); }
#define OUT_FLUSH()     out_pos = 0

glyph* g_full = NULL;

static u32 blank_tex = -1;
static u32 crate_tex = -1;

// ---- func decls ----
void app_key_callback(key _key, input_state state, mod_flags mods);
void app_utf8_callback(int code);
INLINE void cursor_left();
INLINE void cursor_right();
INLINE void cursor_up();
INLINE void cursor_down();

void app_init()
{
  char* _cwd = _getcwd(NULL, 0);
  ASSERT(_cwd != NULL);
  strcpy(cwd, _cwd);
  P_STR(cwd);
  
  app_load_file("assets/examples/text.txt");

  g_full = text_get_glyph(9608);

  input_register_key_callback(app_key_callback);
  input_register_utf8_callback(app_utf8_callback);

  blank_tex = texture_create_from_path("assets/textures/blank.png",
					    "blank", true).handle; 
	
  crate_tex = texture_create_from_path("assets/textures/crate01.png",
					    "crate01", true).handle; 
  int w, h;
  window_get_size(&w, &h);
  P_INT(w);
  P_INT(h);
  float dpi_x, dpi_y;
  window_get_monitor_dpi(&dpi_x, &dpi_y);
  P_F32(dpi_x);
  P_F32(dpi_y);
}

void app_update()
{  
  int w, h;
  window_get_size(&w, &h);
  vec2 c_pos;
  text_screen_pos(cursor, c_pos, buffer, buffer_pos);
  text_draw_glyph_box(c_pos, g_full, RGB_F(0.6f, 0.6f, 0.7f));
  text_draw_screen(buffer, buffer_pos);
  int g_w, g_h;
  text_get_glyph_size_estim(&g_w, &g_h);
  text_draw_line(VEC2_XY(0, -h * 2 + g_h * 2), out, out_pos);
  // text_draw_quad(VEC2_XY(0, -h), VEC2_XY(w, 200), RGB_F_RGB(1));// RGB_F(0.2f, 0.2f, 0.2f));
  if (cmd_len > 1)
  { 
    vec2 pos = VEC2_INIT(0);
    text_line_pos(cmd_len, pos, cmd, cmd_len -1);
    pos[0] += g_full->advance;
    text_draw_glyph_box(VEC2_XY(pos[0], -h * 2 + g_h), 
      g_full/*&cmd[cmd_len -1]*/, RGB_F(0.3f, 0.3f, 0.35f)); 
  }
  // text_draw_line(VEC2_XY(0, -h * 2 + g_h), cmd, cmd_len == 1 ? 0 : cmd_len);
  text_draw_line(VEC2_XY(0, -h * 2 + g_h), cmd, cmd_len);
   
  if (is_key_pressed(KEY_Tab))
	{
		wiref_act = !wiref_act;
		renderer_set_wireframe(wiref_act);
	}
  if (is_key_pressed(KEY_Escape))
  {
    app_quit();  
  }
  if (is_key_pressed(KEY_F11))
  {
    static int w_mode = WINDOW_MIN;
    w_mode = w_mode == WINDOW_MIN ? WINDOW_FULL : WINDOW_MIN;
    window_set_mode(w_mode);  
  }
}

void app_set_cwd(const char* _cwd)
{
  ASSERT(_cwd != NULL);
  strcpy(cwd, _cwd);
  OUT_FILL(cwd);
}
void app_cat_cwd(const char* cat)
{
  ASSERT(cat != NULL);
  strcat(cwd, cat);
  OUT_FILL(cwd);
}
const char* app_get_cwd()
{
  return cwd;
}
void app_new_file()
{
  buffer_pos = 0;
  cursor = 0;
}
void app_load_file(const char* path)
{
  buffer_pos = 0;
  cursor = 0;
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);
  for (int i = 0; i < txt_len; ++i)
  { 
    if (txt[i] == ' ')  
    { buffer[buffer_pos++] = text_get_glyph(U_SPACE); }
    if (txt[i] == '\n') 
    { buffer[buffer_pos++] = text_get_glyph(U_CR); }
    if (isspace(txt[i]))
    { continue; }
    buffer[buffer_pos++] = text_get_glyph(txt[i]); 
  }
  free(txt);
}

void app_fill_out(const char* str)
{
  OUT_FILL(str);
}

void app_utf8_callback(int code)
{
  // PF("pressed: %d | '%c'\n", code, (char)code);
  if (code == CMD_KEY)  { in_cmd = true; CMD_PSH(CMD_START); }
  else if (in_cmd)      { CMD_PSH(code); }
  else
  { 
    text_insert_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor, code); 
    cursor_right();
  }
  

}
void app_key_callback(key _key, input_state state, mod_flags mods)
{
  if (_key == KEY_LeftArrow)  { cursor_left();  }
  if (_key == KEY_RightArrow) { cursor_right(); }
  if (_key == KEY_UpArrow)    { cursor_up();    }
  if (_key == KEY_DownArrow)  { cursor_down();  }

  if (_key == KEY_Enter)
  { 
    if (in_cmd)
    {
     cmd_parse(cmd, cmd_len);
     CMD_FLUSH();
     in_cmd = false;
    }
    else
    { 
      text_insert_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor, U_CR); 
      cursor_right();
    }
  }
  if (_key == KEY_Backspace)
  { 
    if (in_cmd) { CMD_POP(); }
    else 
    { 
      text_remove_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor); 
      cursor_left();
    }

  }
}

INLINE void cursor_left()
{
  cursor = MAX(0, cursor -1);
}
INLINE void cursor_right()
{
  if (buffer[cursor +1]->code == U_CR  || 
      buffer[cursor +1]->code == U_EOF ||
      cursor +1 >= BUFFER_MAX)
  { return; }
  cursor++;
}
INLINE void cursor_up()
{
  int l_space = 0;
  while (cursor > 0                       &&
         buffer[cursor -1]->code != U_CR  &&
         buffer[cursor -1]->code != U_EOF )
  { cursor--; l_space++; }
  cursor -= cursor <= 0 ? 0 : 2;
  // cursor -= 2;
  while (cursor > 0                    && 
         buffer[cursor]->code != U_CR  &&
         buffer[cursor]->code != U_EOF )
  { cursor--; }
  cursor += cursor <= 0 ? 0 : 1;
  while (l_space > 0                  &&
         buffer[cursor]->code != U_CR && 
         buffer[cursor]->code != U_EOF )
  { cursor++; l_space--; }
}
INLINE void cursor_down()
{
  int l_space = 0;
  while (cursor > 0                       &&
         buffer[cursor -1]->code != U_CR  &&
         buffer[cursor -1]->code != U_EOF )
  { cursor--; l_space++; }
  cursor++;
  while (buffer[cursor]->code != U_CR  &&
         buffer[cursor]->code != U_EOF && 
         cursor < buffer_pos -1)
  { cursor++; }
  cursor++;
  while (l_space > 0                  &&
         buffer[cursor]->code != U_CR && 
         buffer[cursor]->code != U_EOF )
  { cursor++; l_space--; }
}


