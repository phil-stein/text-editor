#include "text.h"
#include "../core/window.h"
#include "../math/math_inc.h"

#include <ctype.h>
#include <wchar.h>  // tmp
#include <locale.h> // tmp


#include "../../external/FREETYPE/ft2build.h"
#include FT_FREETYPE_H
#include "../../external/GLAD/glad.h"

FT_Library  library;
FT_Face     face;

int glyph_h = 0;  // rough estimate not acutal size
int glyph_w = 0;  // rough estimate not acutal size


#define GLYPH_POOL_MAX 256
glyph glyph_pool[GLYPH_POOL_MAX];
int   glyph_pool_pos = 0;

// ---- func decls ----
INLINE glyph text_ft_bitmap_to_glyph(FT_Bitmap* b);

// @TODO: load ascii and the use another glyph_pool for the buffered chars

void text_init(const char* font_path, int font_size)
{
  int error = 0;

  error = FT_Init_FreeType( &library );
  FREETYPE_ERR_CHECK(error, "error during library init.");

  error = FT_New_Face(library, font_path, 0, &face);
  FREETYPE_ERR_CHECK(error, "error during face init.");
  
  int w, h;
  get_window_size(&w, &h);
  error = FT_Set_Char_Size(
      face,           // handle to face object           
      0,              // char_width in 1/64th of points, 0: same as height  
      font_size*64,   // char_height in 1/64th of points 
      w,              // horizontal device resolution  (dpi)  
      h );            // vertical device resolution    (dpi)
  FREETYPE_ERR_CHECK(error, "error during face sizing.");

  glyph_w = font_size;  
  glyph_h = font_size * 15; // 14
  
  P("FT inited no errors.");
}

INLINE glyph text_ft_bitmap_to_glyph(FT_Bitmap* b)
{
  glyph g;
  g.w = b->width;
  g.h = b->rows;
  
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glGenTextures(1, &g.handle);
  glBindTexture(GL_TEXTURE_2D, g.handle);  
  
  // no interpolation
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, g.w, g.h, 0, GL_RED, GL_UNSIGNED_BYTE, b->buffer);

  return g;
}

glyph* text_make_glyph(int code)
{
  int error = 0;
  int glyph_index = FT_Get_Char_Index(face, code); // unicode, utf-32
  if (glyph_index == code) { ERR("no charmap found."); }

  error = FT_Load_Glyph(
      face,              // handle to face object 
      glyph_index,        // glyph index           
      FT_LOAD_DEFAULT );  // load flags
  FREETYPE_ERR_CHECK(error, "error loading glyph.");

  if (face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
  {
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL); // aliased 256 gaylevels
    FREETYPE_ERR_CHECK(error, "error rendering glyph.");
  }
  
  int w, h;
  get_window_size(&w, &h);
  // @UNSURE: y1 & y0 / by h 
  const f32 y1 = (f32)(face->glyph->bitmap_top) / w;        // top
  const f32 y0 = (f32)(y1 - face->glyph->bitmap.rows) / w;  // bottom, top - height
  const f32 x0 = (f32)(face->glyph->bitmap_left) / h;       // left
  const f32 x1 = (f32)(x0 + face->glyph->bitmap.width) / h; // right, left + width 
  // const f32 y1 = (f32)(face->glyph->bitmap_top) / h;        // top
  // const f32 y0 = (f32)(y1 - face->glyph->bitmap.rows) / h;  // bottom, top - height
  // const f32 x0 = (f32)(face->glyph->bitmap_left) / w;       // left
  // const f32 x1 = (f32)(x0 + face->glyph->bitmap.width) / w; // right, left + width 

  ASSERT(y1 >= -1 && y1 <= 1);
  ASSERT(y0 >= -1 && y0 <= 1);
  ASSERT(x1 >= -1 && x1 <= 1);
  ASSERT(x0 >= -1 && x0 <= 1);
  P_F32(y1);
  P_F32(y0);
  P_F32(x1);
  P_F32(x0);
  // tmp
  const f32 _y1 = (f32)(face->glyph->bitmap_top);        
  const f32 _y0 = (f32)(y1 - face->glyph->bitmap.rows);  
  const f32 _x0 = (f32)(face->glyph->bitmap_left);       
  const f32 _x1 = (f32)(x0 + face->glyph->bitmap.width);
  P_F32(_y1);
  P_F32(_y0);
  P_F32(_x1);
  P_F32(_x0);
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
  g.advance = face->glyph->advance.x / 64;
  glyph_pool[glyph_pool_pos++] = g; 
  glyph_pool[glyph_pool_pos -1].vao = vao;
  
  return &glyph_pool[glyph_pool_pos -1];
}

glyph* text_get_char_glyph(int code)
{
  for (int i = 0; i < glyph_pool_pos; ++i)
  {
    if (glyph_pool[i].code == code) { return &glyph_pool[i]; }
  }
  return text_make_glyph(code);
}

glyph_info text_get_info(int code)
{
  // if (isspace(code) && code != ' ')
  glyph* ptr = text_get_char_glyph(code);
  glyph_info info;
  info.code = ptr->code;
  info.tex  = ptr->handle;
  info.vao  = ptr->vao;
  info.advance = ptr->advance;
  static int count;
  PF("%.3d | ", count++);
  PF("[%.3d]", code);
  setlocale(LC_CTYPE, "");
  wprintf(L" '%lc'\n", (wchar_t)code);
  return info;
}

void text_get_glyph_size_estim(int* w, int* h)
{
  *w = glyph_w;
  *h = glyph_h;
}


