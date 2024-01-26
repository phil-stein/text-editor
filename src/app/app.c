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

// @TODO: make this dynamic
#define BUFFER_MAX 20000
int buffer[BUFFER_MAX];
int buffer_pos = 0;

int  cursor = 0;    // index into 'buffer'
rgbf cursor_col = { 0.4f, 0.4f, 0.5f };
int  scroll = 0;    // y lines to offset
#define SCROLL_CAP 6

#define CMD_KEY   '>'
#define CMD_START 0x25B6 // 0x25BA // 0x25B8 // '>'/  / 0x2192 
#define CMD_MAX   128
int  cmd[CMD_MAX];
int  cmd_len = 0;
bool in_cmd = false;
int  cmd_cursor = 0;
#define CMD_PSH(c)   if (cmd_len < CMD_MAX) { cmd[cmd_len++] = c;  }
#define CMD_POP()    cmd_len = cmd_len -1 >= 1 ? cmd_len -1 : 1 
#define CMD_INS(p, c)   if (p < CMD_MAX )                                     \
                        { for (int i = p; i < cmd_len && i < CMD_MAX -1; ++i) \
                          { cmd[i +1] = cmd[i]; }                             \
                          cmd[p] = (c); cmd_len++; }
#define CMD_DEL(p)      if (p < CMD_MAX )                      \
                        { for (int i = p; i < cmd_len; i++)    \
                          { cmd[i] = cmd[i +1]; }              \
                          cmd_len--; }
#define CMD_FLUSH()     cmd_len = 0
#define CMD_TRUNC(len)  cmd_len = cmd_len - len >= 1 ? cmd_len - len : 1
#define CMD_CURSOR_L()  cmd_cursor = MAX(0, cmd_cursor -1)        
#define CMD_CURSOR_R()  cmd_cursor = MIN(cmd_len, cmd_cursor +1)  

#define OUT_MAX 64
int   out[OUT_MAX];
int   out_pos = 0;
float out_t = 0.0f;
#define OUT_T_DEFAULT      2.0f
#define OUT_FILL_T(str, t) for (out_pos = 0; out_pos < strlen(str) && out_pos < OUT_MAX; ++out_pos)  \
                              { out[out_pos] = str[out_pos]; } out_t = t
#define OUT_FILL(str)      OUT_FILL_T(str, OUT_T_DEFAULT)
#define OUT_FLUSH()        out_pos = 0

#define STATUS_MAX 64
int status[STATUS_MAX];
int status_pos = 0;
#define STATUS_FILL(str)   for (status_pos = 0; status_pos < strlen(str) && status_pos < STATUS_MAX; ++status_pos)  \
                           { status[status_pos] = str[status_pos]; }
#define STATUS_FLUSH()     status_pos = 0

#define OPEN_PATH_MAX 128
char open_path[OPEN_PATH_MAX];
bool open_is_new = false;

glyph* g_full = NULL;

int line_num_digits = 3;
rgbf line_num_col = { 0.7f, 0.7f, 0.8f };

#define UI_COLOR RGB_F(0.2f, 0.2f, 0.25f)

static u32 blank_tex = -1;
static u32 crate_tex = -1;

#define FONT_SIZE_STEP  1
int  font_size = 14;

font_t font_x = FONT_INIT();
font_t font_s = FONT_INIT();
font_t font_m = FONT_INIT();
font_t font_l = FONT_INIT();
#define FONT_X_SIZE_DIF  -4
#define FONT_S_SIZE_DIF  -2
#define FONT_M_SIZE_DIF   0
#define FONT_L_SIZE_DIF   2

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
  
  char* path = "assets/fonts/JetBrainsMonoNL-Regular.ttf"; // "assets/fonts/CascadiaMono.ttf"
  
  text_load_font(path, font_size + FONT_X_SIZE_DIF, &font_x);
  text_load_font(path, font_size + FONT_S_SIZE_DIF, &font_s);
  text_load_font(path, font_size + FONT_M_SIZE_DIF, &font_m);
  text_load_font(path, font_size + FONT_L_SIZE_DIF, &font_l);

  font_main     = &font_m;
  font_line_nr  = &font_s;
  font_cmd      = &font_m;
  font_out      = &font_m;
  font_status   = &font_m;
  
  text_draw_init(font_main);

  // ------------------

  app_load_file("assets/examples/text.txt");

  g_full = text_get_glyph(U_FULL, font_main);

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
  int line_buf[3];
  int i = 0; while (i <= lines)
  {
    int h = i % 1000 * 0.01f;
    int t = i % 100  * 0.1f;
    int o = i % 10   * 1.0f;
    line_buf[0] = h + U_0 == U_0 ? U_MIDDLE_DOT : h +U_0;   // houndreds place
    line_buf[1] = t + U_0 == U_0 ? U_MIDDLE_DOT : t +U_0;   // tens place
    line_buf[2] = o +U_0;                                   // ones place
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
  
  if (cmd_len > 0)
  { 
    vec2 cmd_ui_pos = { 
      0 + font_cmd->gw,
      -h * 2 + font_cmd->gh - (font_cmd->gh * 0.35f) };
    vec2 cmd_ui_size = {
      (w * 2) - (font_cmd->gw) - (font_status->gw),
      font_cmd->gh * 1.5f  
    };
    text_draw_quad(cmd_ui_pos, cmd_ui_size, UI_COLOR);
    vec2 pos = VEC2_INIT(0);
    text_line_pos(cmd_cursor, pos, cmd, cmd_len -1, font_cmd);
    pos[0] += g_full->advance;
    text_draw_glyph_box(VEC2_XY(pos[0], -h * 2 + g_h), 
      g_full/*&cmd[cmd_len -1]*/, RGB_F(0.3f, 0.3f, 0.35f)); 
    vec2 cmd_pos = { 0, -h * 2 + g_h }; 
    text_draw_line(cmd_pos, cmd, cmd_len, font_cmd);
  }
  
  int _status[STATUS_MAX];
  int _status_pos = status_pos;
  for (int i = 0; i < status_pos; ++i) { _status[i] = status[i]; }
  if (in_cmd)
  { _status[_status_pos++] = ' '; _status[_status_pos++] = '[';
    _status[_status_pos++] = 'c'; _status[_status_pos++] = 'm';
    _status[_status_pos++] = 'd'; _status[_status_pos++] = ']'; }
  else
  { _status[_status_pos++] = ' '; _status[_status_pos++] = '[';
    _status[_status_pos++] = 't'; _status[_status_pos++] = 'x';
    _status[_status_pos++] = 't'; _status[_status_pos++] = ']'; }
  vec2 status_pos = { (2*w) - font_status->gw * (_status_pos +4), -h * 2 + font_status->gh * 2 };
  status_pos[0] -= font_status->gw * 1;
  status_pos[1] -= font_status->gh * 0.55f;
  text_draw_quad(status_pos, VEC2_XY(font_status->gw * (_status_pos +4), font_status->gh * 2), UI_COLOR);
  status_pos[0] += font_status->gw * 1;
  status_pos[1] += font_status->gh * 0.55f;
  text_draw_line(status_pos, _status, _status_pos, font_status);


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
void app_load_file(const char* path)
{
  buffer_pos = 0;
  cursor = 0;
  open_is_new = false;
  strcpy(open_path, path);
  int txt_len = 0;
  char* txt = read_text_file_len(path, &txt_len);
  for (int i = 0; i < txt_len && i < BUFFER_MAX; ++i)
  { 
    if (txt[i] == U_TAB)
    { 
      for (int i = 0; i < TAB_SPACES; ++i) 
      { buffer[buffer_pos++] = U_SPACE; }
      continue;
    }
    if (txt[i] == '\n')
    { txt[i] = U_CR; }
    if (isspace(txt[i])   && 
        txt[i] != U_SPACE &&
        txt[i] != U_CR)
    { continue; }
    buffer[buffer_pos++] = txt[i]; 
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
    if (buffer[i] > 127) { continue; }
    ascii[i] = buffer[i] == U_CR    ? '\n' : 
               buffer[i] == U_EOF   ? '\0' :
               buffer[i] == U_SPACE ? ' '  :
               buffer[i]; 
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
    if (buffer[i] > 127) { continue; }
    ascii[i] = buffer[i] == U_CR    ? '\n' : 
               buffer[i] == U_EOF   ? '\0' :
               buffer[i] == U_SPACE ? ' '  :
               buffer[i]; 
  }
  ascii[buffer_pos +1] = '\0'; // @TODO: do this better
  write_text_file(path, ascii, buffer_pos);
  OUT_FILL(open_path);
}

void app_fill_out(const char* str)
{
  OUT_FILL(str);
}

font_t* app_get_main_font()
{
  return font_main;
}
void app_resize_fonts(int size)
{
  text_set_font_size(size + FONT_X_SIZE_DIF, &font_x);
  text_set_font_size(size + FONT_S_SIZE_DIF, &font_s);
  text_set_font_size(size + FONT_M_SIZE_DIF, &font_m);
  text_set_font_size(size + FONT_L_SIZE_DIF, &font_l);
  font_size = size + FONT_M_SIZE_DIF;
  char buf[24];
  SPRINTF(24, buf, "font size: %d", font_size);
  OUT_FILL(buf);
}

void app_utf8_callback(int code)
{
  if (code == CMD_KEY)  { in_cmd = true; CMD_PSH(CMD_START); }
  else if (in_cmd)      { CMD_PSH(code); CMD_CURSOR_R(); }
  else
  { 
    text_insert_char(buffer, &buffer_pos, BUFFER_MAX, cursor, code); 
    cursor_right();
  } 

}
void app_key_callback(key _key, input_state state, mod_flags mods)
{
  // @TODO: delete, start, end
 
  if (_key == KEY_S && HAS_FLAG(mods, MOD_CTRL))
  { app_save_open_file(); }
  if (_key == KEY__Plus && HAS_FLAG(mods, MOD_CTRL))
  { app_resize_fonts(font_size + FONT_SIZE_STEP); }
  if (_key == KEY__Minus && HAS_FLAG(mods, MOD_CTRL))
  { app_resize_fonts(font_size - FONT_SIZE_STEP); }
 

  if (_key == KEY_Tab && HAS_FLAG(mods, MOD_CTRL))
	{
		// wiref_act = !wiref_act;
		// renderer_set_wireframe(wiref_act);
    P("wireframe deactivated");
    return;
	}
  if (in_cmd)      
  {
    // if (_key == KEY_LeftArrow)  { CMD_CURSOR_L();  }
    // if (_key == KEY_RightArrow) { CMD_CURSOR_R(); }

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
      text_insert_char(buffer, &buffer_pos, BUFFER_MAX, cursor, U_CR); 
      cursor++;
    }
    if (_key == KEY_Backspace && 
        buffer[cursor-1] != U_CR && cursor > 0)
    { 
      text_remove_char(buffer, (int*)&buffer_pos, BUFFER_MAX, cursor); 
      cursor = MAX(0, cursor -1);
    }
    if (_key == KEY_Tab)
    {
      for (int i = 0; i < TAB_SPACES; ++i)
      { 
        text_insert_char(buffer, &buffer_pos, BUFFER_MAX, cursor, U_SPACE); 
        cursor_right();
      } 
    }
  }
}

INLINE void cursor_left()
{
  cursor = MAX(0, cursor -1);
  cursor = buffer[cursor] == U_CR ? cursor +1 : cursor;
}
INLINE void cursor_right()
{
  if (buffer[cursor] == U_CR  || 
      buffer[cursor] == U_EOF ||
      cursor +1 >= BUFFER_MAX)
  { return; }
  cursor++;
}
INLINE void cursor_up()
{
  int l_space = 0;
  while (cursor > 0                       &&
         buffer[cursor -1] != U_CR  &&
         buffer[cursor -1] != U_EOF )
  { cursor--; l_space++; }
  cursor -= cursor <= 0 ? 0 : 2;
  // cursor -= 2;
  while (cursor > 0                    && 
         buffer[cursor] != U_CR  &&
         buffer[cursor] != U_EOF )
  { cursor--; }
  cursor += cursor <= 0 ? 0 : 1;
  while (l_space > 0                  &&
         buffer[cursor] != U_CR && 
         buffer[cursor] != U_EOF )
  { cursor++; l_space--; }
}
INLINE void cursor_down()
{
  int l_space = 0;
  while (cursor > 0                       &&
         buffer[cursor -1] != U_CR  &&
         buffer[cursor -1] != U_EOF )
  { cursor--; l_space++; }
  cursor++;
  while (buffer[cursor] != U_CR  &&
         buffer[cursor] != U_EOF && 
         cursor < buffer_pos -1)
  { cursor++; }
  cursor++;
  while (l_space > 0                  &&
         buffer[cursor] != U_CR && 
         buffer[cursor] != U_EOF )
  { cursor++; l_space--; }
}

INLINE int cursor_line()
{
  int lne = 0;
  for (int i = 0; i < cursor; ++i)
  { lne = buffer[i] == U_CR ? lne +1 : lne; }
  return lne;
}
INLINE int cursor_column()
{
  int col = 0;
  while (buffer[col +1] != U_CR) { col++; }
  return col;
}

