#ifndef TEXT_H
#define TEXT_H

#include <core/core.h>
#include <core/texture.h>
#include <math/math_inc.h>

#include <FREETYPE/ft2build.h>
#include FT_FREETYPE_H

#define FT_ERR_STR(err)                                   \
    (err) == 0x00 ? "no error" :                          \
    (err) == 0x01 ? "cannot open resource" :              \
    (err) == 0x02 ? "unknown file format" :               \
    (err) == 0x03 ? "broken file" :                       \
    (err) == 0x04 ? "invalid freetype version" :          \
    (err) == 0x05 ? "module version too low" :            \
    (err) == 0x06 ? "invalid argument" :                  \
    (err) == 0x07 ? "unimplemented feature" :             \
    (err) == 0x08 ? "broken table" :                      \
    (err) == 0x09 ? "broken offset in table" :            \
    (err) == 0x0A ? "array allocation size too big" :     \
    (err) == 0x0B ? "missing module" :                    \
    (err) == 0x0C ? "missing propertyi" :                 \
    (err) == 0x0D ? "unused error" :                      \
    (err) == 0x0E ? "unused error" :                      \
    (err) == 0x0F ? "unused error" :                      \
    (err) == 0x10 ? "invalid glyph index" :               \
    (err) == 0x11 ? "invalid character code" :            \
    (err) == 0x12 ? "unsupported glyph image format" :    \
    (err) == 0x13 ? "cannot render glyph format" :        \
    (err) == 0x14 ? "invalid outline" :                   \
    (err) == 0x15 ? "invalid composite glyph" :           \
    (err) == 0x16 ? "too many hints" :                    \
    (err) == 0x17 ? "invalid pixel size" :                \
    (err) == 0x18 ? "unused error" :                      \
    (err) == 0x19 ? "unused error" :                      \
    (err) == 0x20 ? "invalid object handle" :             \
    (err) == 0x21 ? "invalid module handle" :             \
    (err) == 0x22 ? "invalid library handle" :            \
    (err) == 0x23 ? "invalid face handle" :               \
    (err) == 0x24 ? "invalid size handle" :               \
    (err) == 0x25 ? "invalid glyph slot handle" :         \
    (err) == 0x26 ? "invalid charmap handle" :            \
    (err) == 0x27 ? "invalid cache manager handle" :      \
    (err) == 0x28 ? "invalid stream handle" :             \
    "unhadled by FT_ERR_STR()" 

#define P_FREETYPE_ERR(err, msg)  printf("[FT_ERROR] [%d|0x%x]'%s', %s\n -> file: '%s', line: %d\n", err, err, FT_ERR_STR(err), msg, __FILE__, __LINE__)

#define FREETYPE_ERR(err, msg)    P_FREETYPE_ERR(err, msg); exit(0)

#define FREETYPE_ERR_CHECK(err, msg) if (err) { FREETYPE_ERR(err, msg); }

// ---- types ----

// @TODO: make this u32[2]
typedef struct glyph_render_info
{
  u32  code;
  u32  advance;
  u32  vao;
  u32  tex;
  rgbf tint;
}glyph;

#define FONT_POOL_MAX 512
typedef struct
{
  FT_Face face;   // freetype font info
  int size;       // current size
  int gw, gh;     // glyph width / height

  glyph pool[FONT_POOL_MAX];
  int pool_pos;

}font_t;

#define FONT_INIT()   { .face = NULL, .size = 0, .gw = 0, .gh = 0, .pool_pos = 0, }  


#define FONT_PATH_MAX   256
#define FONT_NAME_MAX   128
#define GLYPH_POOL_MAX  256


void text_load_font(const char* font_path, int font_size, font_t* font);
void text_set_font_size(int size, font_t* font);
void text_cleanup();
void text_free_font(font_t* font);

glyph* text_make_glyph(int code, font_t* font);
glyph* text_get_glyph(int code, font_t* font);

const char* text_get_font(int* size);

#endif
