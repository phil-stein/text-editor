#ifndef CMD_H
#define CMD_H

#include <text/text_inc.h>


typedef enum cmd_token_type { TOK_NUM, TOK_STR, TOK_PRINT, TOK_FONT, TOK_SIZE, TOK_OPEN, TOK_CWD, TOK_NEW, TOK_SAVE } cmd_token_type;

#define CMD_TOKEN_MAX 32
typedef struct cmd_token
{
  char str[CMD_TOKEN_MAX];
  cmd_token_type type;
}cmd_token;

#define CMD_TOKEN_BLOCK_MAX 12
typedef cmd_token cmd_token_block[CMD_TOKEN_BLOCK_MAX];


void cmd_parse(int codes[], int codes_len);
void cmd_tokenize(int codes[], int codes_len, cmd_token_block block, int* block_len);
void cmd_interpret(cmd_token_block block, int block_len);

#endif
