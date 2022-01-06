#include "cmd.h"
#include "app.h"
#include <core/core.h>
#include <core/window.h>

#include <ctype.h>

void cmd_parse(glyph* codes[], int codes_len)
{
  cmd_token_block block;
  int block_len = 0;
  cmd_tokenize(codes, codes_len, block, &block_len);
  cmd_interpret(block, block_len);
}

void cmd_tokenize(glyph* codes[], int codes_len, cmd_token_block block, int* block_len)
{
  *block_len = 0;
  cmd_token t;
  P_INT(codes_len);
  // start at 1 because '>'
  for (int i = 1; i < codes_len; ++i)
  {
    if (isspace(codes[i]->code)) { continue; }

    if (codes[i +0]->code == 'p' &&
        codes[i +1]->code == 'r' &&
        codes[i +2]->code == 'i' &&
        codes[i +3]->code == 'n' &&
        codes[i +4]->code == 't' )
    {
      t.type = TOK_PRINT;
      block[*block_len] = t;
      *block_len += 1;
      i += 4;
      continue;
    }
    if (codes[i +0]->code == 'f' &&
        codes[i +1]->code == 'o' &&
        codes[i +2]->code == 'n' &&
        codes[i +3]->code == 't' )
    {
      t.type = TOK_FONT;
      block[*block_len] = t;
      *block_len += 1;
      i += 3;
      // PF("added font, next: '%c'\n", codes[i +1].code);
      continue;
    }
    if (codes[i +0]->code == 's' &&
        codes[i +1]->code == 'i' &&
        codes[i +2]->code == 'z' &&
        codes[i +3]->code == 'e' )
    {
      t.type = TOK_SIZE;
      block[*block_len] = t;
      *block_len += 1;
      i += 3;
      // PF("added size, next: '%c'\n", codes[i +1].code);
      continue;
    }
    if (codes[i +0]->code == 'o' &&
        codes[i +1]->code == 'p' &&
        codes[i +2]->code == 'e' &&
        codes[i +3]->code == 'n' )
    {
      t.type = TOK_OPEN;
      block[*block_len] = t;
      *block_len += 1;
      i += 3;
      continue;
    }
    if (codes[i +0]->code == 'c' &&
        codes[i +1]->code == 'w' &&
        codes[i +2]->code == 'd' )
    {
      t.type = TOK_CWD;
      block[*block_len] = t;
      *block_len += 1;
      i += 2;
      continue;
    }
    if (codes[i +0]->code == 'n' &&
        codes[i +1]->code == 'e' &&
        codes[i +2]->code == 'w' )
    {
      t.type = TOK_NEW;
      block[*block_len] = t;
      *block_len += 1;
      i += 2;
      continue;
    }
    if (codes[i +0]->code == 's' &&
        codes[i +1]->code == 'a' &&
        codes[i +2]->code == 'v' &&
        codes[i +3]->code == 'e' )
    {
      t.type = TOK_SAVE;
      block[*block_len] = t;
      *block_len += 1;
      i += 3;
      continue;
    }

    // -- number --
    int str_pos = 0;
    while (isdigit(codes[i]->code))
    {
      t.str[str_pos++] = (char)codes[i]->code;
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
    if (isalpha(codes[i]->code) ||
        codes[i]->code == '/'   || codes[i]->code == '\\')
    {
      while ((isalnum(codes[i]->code) || 
              codes[i]->code == ' '   || codes[i]->code == '_'  || 
              codes[i]->code == '/'   || codes[i]->code == '\\' || 
              codes[i]->code == '.'   || codes[i]->code == ','))
      {
        t.str[str_pos++] = (char)codes[i]->code;
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
      // text_set_font_size(n);
      continue;
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
        int font_size;
        const char* font_name = text_get_font(&font_size);
        PF("print | font: '%s', size: %d\n", font_name, font_size);
        char  buf[40 + 16];
        char* buf_ptr = buf;
        buf_ptr += sprintf(buf_ptr, "font: '%.40s", font_name);
        buf_ptr += sprintf(buf_ptr, strlen(font_name) > 40 ? "...'" : "'"); 
        buf_ptr += sprintf(buf_ptr, ", size: %d",font_size);
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
        // if (block[i +1].type == TOK_STR)
        // { app_load_file(block[i +1].str); continue; }
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
  }

}


