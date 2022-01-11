#include <stdio.h>
#include <assert.h>

#include "text.h" 

INLINE void text_insert_char(int* buf, int* buf_len, int buf_max, int pos, int code)
{
  assert(buf_max > *buf_len +1);
  for (int i = *buf_len; i > pos; --i)
  {
    buf[i] = buf[i -1];
  }
  buf[pos] = code;
  *buf_len += 1;
}

INLINE void text_remove_char(int* buf, int* buf_len, int buf_max, int pos)
{
  assert(buf_max > *buf_len +1);
  for (int i = pos; i < *buf_len; ++i)
  {
    buf[i -1] = buf[i];
  }
  *buf_len -= 1;
}

INLINE int text_block_lines(int* g, int g_len)
{ 
  int l = 0; 
  for (int i = 0; i < g_len; ++i) 
  { if (g[i] == U_CR) { l++; } } 
  return l; 
}

