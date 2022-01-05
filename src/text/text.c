#include "text.h"
#include "text_codes.h"
#include <core/window.h>
#include <math/math_inc.h>

#include <ctype.h>
#include <wchar.h>  // tmp
#include <locale.h> // tmp


#include <FREETYPE/ft2build.h>
#include FT_FREETYPE_H
#include <GLAD/glad.h>

FT_Library  library;
FT_Face     face;

int glyph_h = 0;  // rough estimate not acutal size
int glyph_w = 0;  // rough estimate not acutal size

char cur_font_path[FONT_PATH_MAX];
char cur_font_name[FONT_NAME_MAX];
int  cur_font_size = 0;

glyph glyph_pool[GLYPH_POOL_MAX];
int   glyph_pool_pos = 0;

// ---- func decls ----
INLINE glyph text_ft_bitmap_to_glyph(FT_Bitmap* b);

// @TODO: load ascii and the use another glyph_pool for the buffered chars

void text_init(const char* font_path, int font_size)
{
  text_load_font(font_path, font_size);
  
    PF("-> FT inited\n"); 
  PF("  -> font-name: %s\n",   cur_font_name);
  PF("  -> font-size: %d\n",   cur_font_size);
  PF("  -> font-glyphs: %d\n", (int)face->num_glyphs);
  PF("  -> has color: %s\n",   STR_BOOL(FT_HAS_COLOR(face)));
  PF("  -> is sfnt: %s\n",     STR_BOOL(FT_IS_SFNT(face)));
  PF("  -> scalable: %s\n",    STR_BOOL(FT_IS_SCALABLE(face)));
  PF("  -> has kerning: %s\n", STR_BOOL(FT_HAS_KERNING(face)));
}

void text_load_font(const char* font_path, int font_size)
{
  // -- cleanup --
  FT_Done_Face(face);
  FT_Done_FreeType(library);
  face = NULL;
  library = NULL;
  glyph_pool_pos = 0; 

  // ---- font ----
  int error = 0;

  error = FT_Init_FreeType( &library );
  FREETYPE_ERR_CHECK(error, "error during library init.");

  error = FT_New_Face(library, font_path, 0, &face);
  FREETYPE_ERR_CHECK(error, "error during face init.");
  
  float x, y;
  window_get_monitor_dpi(&x, &y);
  int w, h;
  window_get_size(&w, &h);
  error = FT_Set_Char_Size(
      face,           // handle to face object           
      0,              // char_width in 1/64th of points, 0: same as height  
      font_size*64,   // char_height in 1/64th of points 
      x,              // horizontal device resolution  (dpi)  
      y );            // vertical device resolution    (dpi)
  FREETYPE_ERR_CHECK(error, "error during face sizing.");

  glyph_w = font_size * 2;  
  glyph_h = font_size * 4; // * 1.75f; // 14

  strcpy(cur_font_path, font_path);
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
    cur_font_name[cpy++] = font_path[i];
  }
  cur_font_size = font_size;
}
void text_set_font_size(int size)
{
  P_STR(cur_font_path);
  text_load_font(cur_font_path, size);
}


void text_cleanup()
{
  FT_Done_Face(face);
  FT_Done_FreeType(library);
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

glyph* text_make_glyph(int code)
{
  int error = 0;
  int glyph_index = FT_Get_Char_Index(face, code); // unicode, utf-32
  if (glyph_index == code) { ERR("no charmap found."); }
  
  error = FT_Load_Glyph(
      face,               // handle to face object 
      glyph_index,        // glyph index           
      FT_LOAD_DEFAULT | FT_LOAD_COLOR );  // load flags
  FREETYPE_ERR_CHECK(error, "error loading glyph.");
  
  if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
  {
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL); // aliased 256 gaylevels
    FREETYPE_ERR_CHECK(error, "error rendering glyph.");
  }
  // float x, y;
  // get_monitor_dpi(&x, &y); 
  int w, h;
  window_get_size(&w, &h);
  f32 y1 = face->glyph->bitmap_top;        // top
  f32 y0 = y1 - face->glyph->bitmap.rows;  // bottom, top - height
  f32 x0 = face->glyph->bitmap_left;       // left
  f32 x1 = x0 + face->glyph->bitmap.width; // right, left + width 
  
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

  ASSERT(glyph_pool_pos < GLYPH_POOL_MAX -1);
  glyph g = text_ft_bitmap_to_glyph(&face->glyph->bitmap);
  g.code    = code;
  g.advance = face->glyph->advance.x / 32; // / 64; // @UNCLEAR: should be 64
  glyph_pool[glyph_pool_pos++] = g; 
  glyph_pool[glyph_pool_pos -1].vao = vao;
  
  return &glyph_pool[glyph_pool_pos -1];
}

glyph* text_get_glyph(int code)
{
  for (int i = 0; i < glyph_pool_pos; ++i)
  {
    if (glyph_pool[i].code == code) { return &glyph_pool[i]; }
  }
  return text_make_glyph(code);
}

void text_get_glyph_size_estim(int* w, int* h)
{
  *w = glyph_w;
  *h = glyph_h;
}

const char* text_get_font(int* size)
{
  *size = cur_font_size;
  return cur_font_name;
}

