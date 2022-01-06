#include "app.h"
#include "cmd.h"
#include <core/input.h>
#include <core/renderer.h>
#include <core/time.h>
#include <core/file_io.h>
#include <core/window.h>
#include <math/math_inc.h>

#include <ctype.h>
#include <direct.h>


#define TAB_SPACES 2

bool wiref_act = false;

#define CWD_MAX 128
char cwd[CWD_MAX];

#define BUFFER_MAX 512
glyph* buffer[BUFFER_MAX];
u32    buffer_pos = 0;

int cursor = 0;    // index into 'buffer'
rgbf cursor_col = { 0.4f, 0.4f, 0.5f };
int scroll = 0;    // y lines to offset
#define SCROLL_CAP 6

#define CMD_KEY   '>'
#define CMD_START     0x25B6 // 0x25BA // 0x25B8 // '>'/  / 0x2192 
#define CMD_MAX   128
glyph* cmd[CMD_MAX];
int    cmd_len = 0;
bool   in_cmd = false;
int    cmd_cursor = 0;
#define CMD_PSH(c)   if (cmd_len < CMD_MAX) { cmd[cmd_len++] = text_get_glyph(c, font_cmd);  }
#define CMD_POP()    cmd_len = cmd_len -1 >= 1 ? cmd_len -1 : 1 
#define CMD_INS(p, c)   if (p < CMD_MAX )                                     \
                        { for (int i = p; i < cmd_len && i < CMD_MAX -1; ++i) \
                          { cmd[i +1] = cmd[i]; }                             \
                          cmd[p]->code = (c); cmd_len++; } else {P("over");}
#define CMD_DEL(p)      if (p < CMD_MAX )                      \
                        { for (int i = p; i < cmd_len; i++)    \
                          { cmd[i] = cmd[i +1]; }              \
                          cmd_len--; }
#define CMD_FLUSH()     cmd_len = 0
#define CMD_TRUNC(len)  cmd_len = cmd_len - len >= 1 ? cmd_len - len : 1
#define CMD_CURSOR_L()  cmd_cursor = MAX(0, cmd_cursor -1)       ; P_INT(cmd_cursor) 
#define CMD_CURSOR_R()  cmd_cursor = MIN(cmd_len, cmd_cursor +1) ; P_INT(cmd_cursor) 

#define OUT_MAX 64
glyph* out[OUT_MAX];
int    out_pos = 0;
float  out_t = 0.0f;
#define OUT_T_DEFAULT      2.0f
#define OUT_FILL_T(str, t) for (out_pos = 0; out_pos < strlen(str) && out_pos < OUT_MAX; ++out_pos)  \
                              { out[out_pos] = text_get_glyph(str[out_pos], font_out); } out_t = t
#define OUT_FILL(str)      OUT_FILL_T(str, OUT_T_DEFAULT)
#define OUT_FLUSH()        out_pos = 0

#define STATUS_MAX 64
glyph* status[STATUS_MAX];
int    status_pos = 0;
#define STATUS_FILL(str)   for (status_pos = 0; status_pos < strlen(str) && status_pos < STATUS_MAX; ++status_pos)  \
                              { status[status_pos] = text_get_glyph(str[status_pos], font_status); }
#define STATUS_FLUSH()     status_pos = 0

#define OPEN_PATH_MAX 128
char open_path[OPEN_PATH_MAX];
bool open_is_new = false;

glyph* g_full = NULL;

int line_num_digits = 3;
rgbf line_num_col = { 0.7f, 0.7f, 0.8f };

// static u32 blank_tex = -1;
// static u32 crate_tex = -1;

font_t font_s;
font_t font_m;
font_t font_x;

font_t* font_main;
font_t* font_line_nr;
font_t* font_cmd;
font_t* font_out;
font_t* font_status;

// ---- func decls ----
void app_key_callback(key _key, input_state state, mod_flags mods);
void app_utf8_callback(int code);
INLINE void cursor_left();
INLINE void cursor_right();
INLINE void cursor_up();
INLINE void cursor_down();
INLINE int  cursor_line();
INLINE int  cursor_column();

void app_init()
{
  char* _cwd = _getcwd(NULL, 0);
  ASSERT(_cwd != NULL);
  strcpy(cwd, _cwd);
  P_STR(cwd);
  
  // ---- text init ---
  
  text_load_font(
"assets/fonts/\
JetBrains Mono NL Regular \
Nerd Font Complete Mono Windows Compatible.ttf", 
      11, &font_m);
  // P("-> text_init() finished");
 
  text_load_font("assets/fonts/CascadiaMono.ttf",
      16, &font_x);

  text_load_font("assets/fonts/CascadiaMonoItalic.ttf",
      10, &font_s);

  font_main     = &font_m;
  font_line_nr  = &font_s;
  font_cmd      = &font_s;
  font_out      = &font_m;
  font_status   = &font_x;
  
  text_draw_init(font_main);
  // P("-> text_draw_init() finished");

  // ------------------

  app_load_file("assets/examples/text.txt", font_main);

  g_full = text_get_glyph(U_FULL, font_main);

  input_register_key_callback(app_key_callback);
  input_register_utf8_callback(app_utf8_callback);

  // blank_tex = texture_create_from_path("assets/textures/blank.png",
	// 				    "blank", true).handle; 
	// 
  // crate_tex = texture_create_from_path("assets/textures/crate01.png",
  // 				    "crate01", true).handle; 
  
  int w, h;
  window_get_size(&w, &h);
  P_INT(w);
  P_INT(h);
  float dpi_x, dpi_y;
  window_get_monitor_dpi(&dpi_x, &dpi_y);
  P_F32(dpi_x);
  P_F32(dpi_y);
}

void app_update(float dt)
{  
  out_t -= dt;
  if (out_t <= 0.0f)  { OUT_FLUSH(); }

  int w, h;
  window_get_size(&w, &h);
  
  int g_w = font_main->gw;
  int g_h = font_main->gh;

  int screen_lines = (h*2) / g_h;
  scroll = cursor_line() - scroll > (screen_lines - SCROLL_CAP) ? 
           -(cursor_line() - (screen_lines - SCROLL_CAP)) :
           scroll;
  int scroll_y = scroll * g_h;

  int lines = text_block_lines(buffer, buffer_pos);
  glyph* line_buf[3];
  int i = 0; while (i <= lines)
  {
    int h = i % 1000 * 0.01f;
    int t = i % 100  * 0.1f;
    int o = i % 10   * 1.0f;
    line_buf[0] = text_get_glyph(h + U_0 == U_0 ? U_MIDDLE_DOT : h +U_0, font_line_nr);   // houndreds place
    line_buf[1] = text_get_glyph(t + U_0 == U_0 ? U_MIDDLE_DOT : t +U_0, font_line_nr);   // tens place
    line_buf[2] = text_get_glyph(o +U_0, font_line_nr);                                   // ones place
    // use font_s chars and font_m spacing
    text_draw_line_col(VEC2_XY(0, -scroll_y -((g_h * i) + g_h)), line_buf, 3, line_num_col, font_line_nr); 
    i++; 
  }
  int l_offs = line_num_digits * g_w +g_w;
  
  vec2 c_pos;
  text_block_pos(cursor, c_pos, buffer, buffer_pos, font_main);
  c_pos[0] += l_offs;
  c_pos[1] -= scroll_y;
  text_draw_glyph_box(c_pos, g_full, cursor_col);

  text_draw_block(VEC2_XY(l_offs, -scroll_y), buffer, buffer_pos, font_main);
  text_draw_line(VEC2_XY(0, -h * 2 + g_h * 2), out, out_pos, font_out);
  text_draw_line(VEC2_XY( (2*w) - font_status->gw*(status_pos +4), -h * 2 + font_status->gh * 2), status, status_pos, font_status);
  // text_draw_quad(VEC2_XY(0, -h), VEC2_XY(w, 200), RGB_F_RGB(1));// RGB_F(0.2f, 0.2f, 0.2f));
  
  if (cmd_len >= 1)
  { 
    vec2 pos = VEC2_INIT(0);
    text_line_pos(cmd_cursor, pos, cmd, cmd_len -1, font_cmd);
    pos[0] += g_full->advance;
    text_draw_glyph_box(VEC2_XY(pos[0], -h * 2 + g_h), 
      g_full/*&cmd[cmd_len -1]*/, RGB_F(0.3f, 0.3f, 0.35f)); 
    text_draw_line(VEC2_XY(0, -h * 2 + g_h), cmd, cmd_len, font_cmd);
  }
  // text_draw_line(VEC2_XY(0, -h * 2 + g_h), cmd, cmd_len == 1 ? 0 : cmd_len);
  
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
  if (is_key_pressed(KEY_Numpad0)) { scroll++; }
  if (is_key_pressed(KEY_Numpad1)) { scroll--; }
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
  open_is_new = true;
  STATUS_FILL("~ new file ~");
}
void app_load_file(const char* path, font_t* font)
{
  buffer_pos = 0;
  cursor = 0;
  open_is_new = false;
  strcpy(open_path, path);
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);
  for (int i = 0; i < txt_len; ++i)
  { 
    if (txt[i] == ' ')  
    { buffer[buffer_pos++] = text_get_glyph(U_SPACE, font); }
    if (txt[i] == '\n') 
    { buffer[buffer_pos++] = text_get_glyph(U_CR, font); }
    if (isspace(txt[i]))
    { continue; }
    buffer[buffer_pos++] = text_get_glyph(txt[i], font); 
  }
  free(txt);
  char name[32];
  int off = 0;
  for (off = strlen(path); off >= 0; --off)
  { if (path[off] == '\\' || path[off] == '/') { break; } }
  strcpy(name, path + off +1);
  STATUS_FILL(name);
}
void app_save_open_file()
{
  if (open_is_new) { ERR("open file is new"); }
  char ascii[BUFFER_MAX];
  for (int i = 0; i < buffer_pos; ++i)
  {
    if (buffer[i]->code > 127) { continue; }
    ascii[i] = buffer[i]->code == U_CR    ? '\n' : 
               buffer[i]->code == U_EOF   ? '\0' :
               buffer[i]->code == U_SPACE ? ' '  :
               buffer[i]->code; 
  }
  ascii[buffer_pos +1] = '\0'; // @TODO: do this better
  write_text_file(open_path, ascii, buffer_pos);
  OUT_FILL(open_path);
}
void app_save_open_file_as(const char* path)
{
  char ascii[BUFFER_MAX];
  for (int i = 0; i < buffer_pos; ++i)
  {
    if (buffer[i]->code > 127) { continue; }
    ascii[i] = buffer[i]->code == U_CR    ? '\n' : 
               buffer[i]->code == U_EOF   ? '\0' :
               buffer[i]->code == U_SPACE ? ' '  :
               buffer[i]->code; 
  }
  ascii[buffer_pos +1] = '\0'; // @TODO: do this better
  write_text_file(path, ascii, buffer_pos);
  OUT_FILL(open_path);
}

void app_fill_out(const char* str)
{
  OUT_FILL(str);
}

void app_utf8_callback(int code)
{
  // PF("pressed: %d | '%c'\n", code, (char)code);
  if (code == CMD_KEY)  { in_cmd = true; CMD_PSH(CMD_START); }
  else if (in_cmd)      { CMD_PSH(code); CMD_CURSOR_R(); }
  else
  { 
    text_insert_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor, code, font_main); 
    cursor_right();
  } 

}
void app_key_callback(key _key, input_state state, mod_flags mods)
{
  // @TODO: tab, delete, start, end

  if (_key == KEY_Tab && HAS_FLAG(mods, MOD_CTRL))
	{
		// wiref_act = !wiref_act;
		// renderer_set_wireframe(wiref_act);
    P("wireframe deactivated");
    return;
	}
  if (in_cmd)      
  {
    if (_key == KEY_LeftArrow)  { CMD_CURSOR_L();  }
    if (_key == KEY_RightArrow) { CMD_CURSOR_R(); }

    if (_key == KEY_Enter)
    { 
      cmd_parse(cmd, cmd_len);
      CMD_FLUSH();
      cmd_cursor = 0;
      in_cmd = false;
    }
    if (_key == KEY_Backspace)
    { CMD_POP(); CMD_CURSOR_L(); }
    if (_key == KEY_Tab)
    { 
      for (int i = 0; i < TAB_SPACES; ++i) 
      { CMD_PSH(U_SPACE); CMD_CURSOR_R(); } 
    }

  }
  else 
  {
    if (_key == KEY_LeftArrow)  { cursor_left();  }
    if (_key == KEY_RightArrow) { cursor_right(); }
    if (_key == KEY_UpArrow)    { cursor_up();    }
    if (_key == KEY_DownArrow)  { cursor_down();  }

    if (_key == KEY_Enter)
    { 
      text_insert_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor, U_CR, font_main); 
      cursor++;
    }
    if (_key == KEY_Backspace)
    { 
      text_remove_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor); 
      cursor_left();
    }
    if (_key == KEY_Tab)
    {
      for (int i = 0; i < TAB_SPACES; ++i)
      { 
        text_insert_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor, U_SPACE, font_main); 
        cursor_right();
      } 
    }
  }
}

INLINE void cursor_left()
{
  cursor = MAX(0, cursor -1);
}
INLINE void cursor_right()
{
  if (buffer[cursor]->code == U_CR  || 
      buffer[cursor]->code == U_EOF ||
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

INLINE int cursor_line()
{
  int lne = 0;
  for (int i = 0; i < cursor; ++i)
  { lne = buffer[i]->code == U_CR ? lne +1 : lne; }
  return lne;
}
INLINE int cursor_column()
{
  int col = 0;
  while (buffer[col +1]->code != U_CR) { col++; }
  return col;
}

