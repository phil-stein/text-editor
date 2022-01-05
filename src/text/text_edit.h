#include <stdio.h>
#include <assert.h>

#include "text.h" 

// static to avoid duplication when including header multiple times
// forced inline to always inline the functions
#define TEXT_E_INLINE static inline __attribute((always_inline))

TEXT_E_INLINE void text_insert_char(glyph** buf, int* buf_len, int buf_max, int pos, int code)
{
  assert(buf_max > *buf_len +1);
  for (int i = *buf_len; i > pos; --i)
  {
    buf[i] = buf[i -1];
  }
  buf[pos] = text_get_glyph(code);
  *buf_len += 1;
}

TEXT_E_INLINE void text_remove_char(glyph** buf, int* buf_len, int buf_max, int pos)
{
  assert(buf_max > *buf_len +1);
  for (int i = pos; i < *buf_len; ++i)
  {
    buf[i -1] = buf[i];
  }
  *buf_len -= 1;
}
