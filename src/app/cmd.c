#include "cmd.h"
#include "app.h"

#include <global/global.h>
#include <core/window.h>

#include <ctype.h>

static app_data_t* app_data = NULL;

void cmd_init()
{
  app_data = app_data_get();
}

void cmd_parse(int codes[], int codes_len)
{
  cmd_token_block block;
  int block_len = 0;
  cmd_tokenize(codes, codes_len, block, &block_len);
  cmd_interpret(block, block_len);
}

void cmd_tokenize(int codes[], int codes_len, cmd_token_block block, int* block_len)
{
  *block_len = 0;
  cmd_token t;
  P_INT(codes_len);

  #define ADD_TOK(tok, len)   \
      t.type = (tok);         \
      block[*block_len] = t;  \
      *block_len += 1;        \
      i += (len);

  #define IS_SHORT(num)             \
      (*block_len <= i + (num) ||   \
       isspace(codes[i +num]))

      // start at 1 because '>'
  for (int i = 1; i < codes_len; ++i)
  {
    if (isspace(codes[i])) { continue; }

    // -- shorthands --
    
    if (codes[i +0] == 'w' && IS_SHORT(1))
    { ADD_TOK(TOK_SAVE, 0); continue; }

    if (codes[i +0] == 'q' && IS_SHORT(1))
    { ADD_TOK(TOK_QUIT, 0); continue; }


    if (codes[i +0] == 'w' &&
        codes[i +1] == 'q' && IS_SHORT(2))
    { ADD_TOK(TOK_SAVE, 0); ADD_TOK(TOK_QUIT, 1); continue; }

    if (codes[i +0] == 'o' && IS_SHORT(1))
    { ADD_TOK(TOK_OPEN, 0); continue; }

    // -- keywords --
    
    if (codes[i +0] == 'p' &&
        codes[i +1] == 'r' &&
        codes[i +2] == 'i' &&
        codes[i +3] == 'n' &&
        codes[i +4] == 't' )
    { ADD_TOK(TOK_PRINT, 4); continue; }
    if (codes[i +0] == 'f' &&
        codes[i +1] == 'o' &&
        codes[i +2] == 'n' &&
        codes[i +3] == 't' )
    { ADD_TOK(TOK_FONT, 3); continue; }
    if (codes[i +0] == 's' &&
        codes[i +1] == 'i' &&
        codes[i +2] == 'z' &&
        codes[i +3] == 'e' )
    { ADD_TOK(TOK_SIZE, 3); continue; }
    if (codes[i +0] == 'o' &&
        codes[i +1] == 'p' &&
        codes[i +2] == 'e' &&
        codes[i +3] == 'n' )
    { ADD_TOK(TOK_OPEN, 3); continue; }
    if (codes[i +0] == 'c' &&
        codes[i +1] == 'w' &&
        codes[i +2] == 'd' )
    { ADD_TOK(TOK_CWD, 2); continue; }
    if (codes[i +0] == 'n' &&
        codes[i +1] == 'e' &&
        codes[i +2] == 'w' )
    { ADD_TOK(TOK_NEW, 2); continue; }
    if (codes[i +0] == 's' &&
        codes[i +1] == 'a' &&
        codes[i +2] == 'v' &&
        codes[i +3] == 'e' )
    { ADD_TOK(TOK_SAVE, 3); continue; }
    if (codes[i +0] == 'c' &&
        codes[i +1] == 'l' &&
        codes[i +2] == 'o' &&
        codes[i +3] == 's' &&
        codes[i +4] == 'e' )
    { ADD_TOK(TOK_QUIT, 4); continue; }
    if (codes[i +0] == 'q' &&
        codes[i +1] == 'u' &&
        codes[i +2] == 'i' &&
        codes[i +3] == 't')
    { ADD_TOK(TOK_QUIT, 3); continue; }
    if (codes[i +0] == 'h' &&
        codes[i +1] == 'e' &&
        codes[i +2] == 'l' &&
        codes[i +3] == 'p' )
    { ADD_TOK(TOK_HELP, 3); continue; }
    if (codes[i +0] == 'f' &&
        codes[i +1] == 'i' &&
        codes[i +2] == 'l' &&
        codes[i +3] == 'e' &&
        codes[i +4] == 's' )
    { ADD_TOK(TOK_FILES, 4); continue; }
    
    // -- number --
    int str_pos = 0;
    while (isdigit(codes[i]))
    {
      t.str[str_pos++] = (char)codes[i];
      i++;
      if (str_pos >= CMD_TOKEN_MAX) { break; }
    }
    t.str[str_pos] = '\0';
    if (str_pos > 0)
    {
      t.type = TOK_NUM;
      block[(*block_len)++] = t;
      i--;
      continue;
    }

    // -- string --
    str_pos = 0;
    if (isalpha(codes[i]) ||
        codes[i] == '/'   || codes[i]  == '\\')
    {
      while ((isalnum(codes[i]) || 
              codes[i] == ' '   || codes[i]  == '_'  || 
              codes[i] == '/'   || codes[i]  == '\\' || 
              codes[i] == '.'   || codes[i]  == ','))
      {
        t.str[str_pos++] = (char)codes[i];
        i++;
        if (str_pos >= CMD_TOKEN_MAX || i >= codes_len)
        { P("- br -");break; }
      }
      t.str[str_pos] = '\0';
      t.type = TOK_STR;
      block[(*block_len)++] = t;
      i--;
      continue;
    }

  }

  #undef TOK_ADD 
  #undef IS_SHORT
}
void cmd_interpret(cmd_token_block block, int block_len)
{
  // P_INT(block_len);
  
  // @DEBUG:
  for (int i = 0; i < block_len; ++i)
  {
    if (block[i].type == TOK_FONT)
    { P("-- FONT --"); continue; }
    if (block[i].type == TOK_SIZE)
    { P("-- SIZE --"); continue; }
    if (block[i].type == TOK_PRINT)
    { P("-- PRINT --"); continue; }
    if (block[i].type == TOK_CWD)
    { P("-- CWD --"); continue; }
    if (block[i].type == TOK_NEW)
    { P("-- NEW --"); continue; }
    if (block[i].type == TOK_SAVE)
    { P("-- SAVE --"); continue; }
    if (block[i].type == TOK_HELP)
    { P("-- HELP --"); continue; }
    if (block[i].type == TOK_FILES)
    { P("-- FILES --"); continue; }
    if (block[i].type == TOK_NUM)
    {
      int num = strtol(block[i].str, NULL, 10);
      PF("-- [%d] --\n", num);
      continue;
    }
    if (block[i].type == TOK_STR)
    {
      PF("-- '%s' --\n", block[i].str);
    }
  }
  // actual interpreting
  for (int i = 0; i < block_len; ++i)
  {
    if (block[i +0].type == TOK_FONT &&
        block[i +1].type == TOK_SIZE && 
        block[i +2].type == TOK_NUM)
    {
      // set font size
      // text_load_font(font_path, font_size);
      P("-- pre --");
      int n = strtol(block[i +2].str, NULL, 10);
      P_INT(n);
      P("-- post --");
      app_resize_fonts(n);
      continue;
    }
    if (block[i +0].type == TOK_HELP)
    {
      char buf[128];
      SPRINTF(128, buf, 
          "help, print (size / font / cwd / 'str'), close / quit, font size 'num', cwd 'str', open 'str', save"); 
      app_fill_out(buf);
    }
    if (block[i +0].type == TOK_PRINT)
    {
      if (block[i +1].type == TOK_STR || 
          block[i +1].type == TOK_NUM)
      {
        PF("print | %s\n", block[i +1].str);
        app_fill_out(block[i +1].str);
        continue;
      }
      if (block[i +1].type == TOK_SIZE)
      {
        int w, h;
        window_get_size(&w, &h);
        PF("print | width: %d, height: %d\n", w, h);
        SPRINTF(CMD_TOKEN_MAX, block[i +1].str, "width: %d, height: %d", w, h); 
        app_fill_out(block[i +1].str);
        continue;
      }
      if (block[i +1].type == TOK_FONT)
      {
        font_t* font = app_get_main_font();
        PF("print | font: '%s', size: %d\n", font->name, font->size);
        char  buf[40 + 16];
        char* buf_ptr = buf;
        buf_ptr += sprintf(buf_ptr, "font: '%.40s", font->name);
        buf_ptr += sprintf(buf_ptr, strlen(font->name) > 40 ? "...'" : "'"); 
        buf_ptr += sprintf(buf_ptr, ", size: %d",font->size);
        app_fill_out(buf);
        continue;
      }
      if (block[i +1].type == TOK_CWD)
      {
        char buf[64];
        PF("print | cwd: '%s'\n", app_get_cwd());
        SPRINTF(64, buf, "cwd: '%s'", app_get_cwd()); 
        app_fill_out(buf);
        continue;
      }
    }
    if (block[i +0].type == TOK_OPEN)
    {
        if (block[i +1].type == TOK_NEW)
        { app_new_file(); continue; }
        if (block[i +1].type == TOK_STR)
        { app_load_file(block[i +1].str); continue; }
    }
    if (block[i +0].type == TOK_SAVE)
    {
      if (block[i +1].type == TOK_STR)
      {
        app_save_open_file_as(block[i +1].str);
        continue;
      }
      app_save_open_file();  
      continue;
    }
    if (block[i +0].type == TOK_CWD &&
        block[i +1].type == TOK_STR)
    {
      if (block[i +1].str[0] == '/' ||
          block[i +1].str[0] == '\\')
      { 
        app_set_cwd(block[i +1].str);
        continue;
      }
      else
      { 
        char _cwd[64];
        sprintf(_cwd, "\\%s", block[i +1].str);
        app_cat_cwd(_cwd); 
        continue;
      }
    }
    if (block[i +0].type == TOK_FILES)
    {
      app_data->show_files_ui = !app_data->show_files_ui;
    }
    if (block[i +0].type == TOK_QUIT)
    {
      app_quit();
    }
  }

}


