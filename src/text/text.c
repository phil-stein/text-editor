#include "text.h"
#include "text_codes.h"
#include <core/window.h>
#include <math/math_inc.h>

#include <ctype.h>
#include <wchar.h>  // tmp
#include <locale.h> // tmp

#include <GLAD/glad.h>


FT_Library  library;

/// ---- func decls ----
INLINE glyph text_ft_bitmap_to_glyph(FT_Bitmap* b);

// @TODO: doesnt completely clean font
void text_load_font(const char* _font_path, int font_size, font_t* font)
{
  // copy before cleaning font
  char font_path[FONT_PATH_MAX];
  strcpy(font_path, _font_path);

  // -- cleanup --
  // FT_Done_Face(font->face);
  // *font = (font_t)FONT_INIT();
  // font->face = NULL;
  // font->pool_pos = 0; 
  FONT_RESET(font);

  // ---- font ----
  int error = 0;

  error = FT_Init_FreeType( &library );
  FREETYPE_ERR_CHECK(error, "error during library init.");

  P_STR(font_path);
  
  error = FT_New_Face(library, font_path, 0, &font->face);
  FREETYPE_ERR_CHECK(error, "error during face init.");
  
  float x, y;
  window_get_monitor_dpi(&x, &y);
  int w, h;
  window_get_size(&w, &h);
  error = FT_Set_Char_Size(
      font->face,     // handle to face object           
      0,              // char_width in 1/64th of points, 0: same as height  
      font_size*64,   // char_height in 1/64th of points 
      x,              // horizontal device resolution  (dpi)  
      y );            // vertical device resolution    (dpi)
  FREETYPE_ERR_CHECK(error, "error during face sizing.");

  font->gw = font_size * 2;
  font->gh = font_size * 4;

  // -- load ascii --
  for (int i = U_SPACE; i <= 127; ++i)
  { text_make_glyph(i, font); }
  // ----------------

  strcpy(font->path, font_path);
  // get end of path / start of file name
  int pos = 0;
  for (int i = strlen(font_path) -1; i >= 0; --i)
  {
    if (font_path[i] == '\\' || font_path[i] == '/')
    { pos = i +1; break; }
  } 
  int cpy = 0;
  for (int i = pos; i < strlen(font_path) && i < FONT_NAME_MAX; ++i)
  {
    font->name[cpy++] = font_path[i];
  }
  font->name[cpy] = '\0';
  font->size    = font_size;

  PF("-> font loaded\n"); 
  PF("  -> font-name: %s\n",   font->name);
  PF("  -> font-size: %d\n",   font->size);
  PF("  -> font-glyphs: %d\n", (int)font->face->num_glyphs);
  PF("  -> has color: %s\n",   STR_BOOL(FT_HAS_COLOR(font->face)));
  PF("  -> is sfnt: %s\n",     STR_BOOL(FT_IS_SFNT(font->face)));
  PF("  -> scalable: %s\n",    STR_BOOL(FT_IS_SCALABLE(font->face)));
  PF("  -> has kerning: %s\n", STR_BOOL(FT_HAS_KERNING(font->face)));
}
void text_set_font_size(int size, font_t* font)
{
  P_STR(font->path);
  text_load_font(font->path, size, font);
}


void text_cleanup()
{
  FT_Done_FreeType(library);
}
void text_free_font(font_t* font)
{
  FT_Done_Face(font->face);
}

INLINE glyph text_ft_bitmap_to_glyph(FT_Bitmap* b)
{
  glyph g;
  // g.w = b->width;
  // g.h = b->rows;
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &g.tex);
  glBindTexture(GL_TEXTURE_2D, g.tex);  
  
  // no interpolation
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, b->width, b->rows, 0, GL_RED, GL_UNSIGNED_BYTE, b->buffer);

  return g;
}

glyph* text_make_glyph(int code, font_t* font)
{
  int error = 0;
  int glyph_index = FT_Get_Char_Index(font->face, code); // unicode, utf-32
  if (glyph_index == code) { ERR("no charmap found."); }
  
  error = FT_Load_Glyph(
      font->face,         // handle to face object 
      glyph_index,        // glyph index           
      FT_LOAD_DEFAULT | FT_LOAD_COLOR );  // load flags
  FREETYPE_ERR_CHECK(error, "error loading glyph.");
  
  if (font->face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
  {
    error = FT_Render_Glyph(font->face->glyph, FT_RENDER_MODE_NORMAL); // aliased 256 gaylevels
    FREETYPE_ERR_CHECK(error, "error rendering glyph.");
  }
  // float x, y;
  // get_monitor_dpi(&x, &y); 
  int w, h;
  window_get_size(&w, &h);
  f32 y1 = font->face->glyph->bitmap_top;        // top
  f32 y0 = y1 - font->face->glyph->bitmap.rows;  // bottom, top - height
  f32 x0 = font->face->glyph->bitmap_left;       // left
  f32 x1 = x0 + font->face->glyph->bitmap.width; // right, left + width 
  
  f32 verts[] = 
	{ 
	// positions   // tex coords
	 x0, y1,  0.0f, 1.0f,
	 x0, y0,  0.0f, 0.0f,
	 x1, y0,  1.0f, 0.0f,

	 x1, y0,  1.0f, 0.0f,
	 x1, y1,  1.0f, 1.0f,
	 x0, y1,  0.0f, 1.0f,
	};

	u32 vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(f32), &verts, GL_STATIC_DRAW); // quad_verts is 24 long
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32)));

  glyph g   = text_ft_bitmap_to_glyph(&font->face->glyph->bitmap);
  g.code    = code;
  g.advance = font->face->glyph->advance.x / 32; // / 64; // @UNCLEAR: should be 64
  g.vbo     = vbo;
  g.vao     = vao;
  
  ASSERT(font->pool_pos < FONT_POOL_MAX -1);
  font->pool[font->pool_pos++] = g; 
  
  return &font->pool[font->pool_pos -1];
}

glyph* text_get_glyph(int code, font_t* font)
{
  for (int i = 0; i < font->pool_pos; ++i)
  {
    if (font->pool[i].code == code) { return &font->pool[i]; }
  }
  return text_make_glyph(code, font);
}


