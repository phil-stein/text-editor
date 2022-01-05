#include "text_draw.h"
#include "text_codes.h"
#include <core/window.h>
#include <core/shader.h>
#include <core/input.h>
// #include <core/renderer.h> // tmp

#include <GLAD/glad.h>


static int w = 0;  // screen width
static int h = 0;  // screen height

// @TODO: make not use text shader but u32  
shader text_shader;
shader img_shader;
unsigned int blank_tex = 0;

bool glyph_box_act = false;

u32 img_vao;

static glyph* g_full = NULL;

void text_draw_init()
{
	// ---- quad -----
	// f32 quad_verts_original[] = 
	// { 
	// // positions   // tex coords
	//  0.0f,  1.0f,  0.0f, 1.0f,
	//  0.0f,  0.0f,  0.0f, 0.0f,
	//  1.0f,  0.0f,  1.0f, 0.0f,

	//  1.0f,  0.0f,  1.0f, 0.0f,
	//  1.0f,  1.0f,  1.0f, 1.0f,
	//  0.0f,  1.0f,  0.0f, 1.0f,
	// };
	f32 quad_verts[] = 
	{ 
	// positions   // tex coords
	 -1.0f,  1.0f,  0.0f, 1.0f,
	 -1.0f, -1.0f,  0.0f, 0.0f,
	  1.0f, -1.0f,  1.0f, 0.0f,

	  1.0f, -1.0f,  1.0f, 0.0f,
	  1.0f,  1.0f,  1.0f, 1.0f,
	 -1.0f,  1.0f,  0.0f, 1.0f,
	};
	// screen quad VAO
  u32 vbo;
	glGenVertexArrays(1, &img_vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(img_vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(f32), &quad_verts, GL_STATIC_DRAW); // quad_verts is 24 long
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32)));

  text_shader = create_shader_from_file("assets/shaders/text.vert", 
					    "assets/shaders/text.frag", 
					    "text_shader");
  
  img_shader  = create_shader_from_file("assets/shaders/text.vert", 
					    "assets/shaders/basic.frag", 
					    "text_img_shader");
  
  blank_tex = texture_create_from_path("assets/textures/blank.png",
					    "blank", true).handle; 
  
  g_full = text_get_glyph(9608);
 
}

void text_draw_update()
{
  if (is_key_pressed(KEY_LeftAlt))  // KEY_Enter
  { glyph_box_act = !glyph_box_act; } 
}

// ---- single draw ----

void text_draw_glyph(vec2 pos, glyph* g)
{
window_get_size(&w, &h);
	   
  // ---- shader & draw call -----	  
  vec2 _pos;
  vec2_copy(pos, _pos);
  _pos[0] -= w; 
  _pos[1] += h; 
  _pos[0] *= 0.5f;
  _pos[1] *= 0.5f;
  vec2 _size;
  vec2_copy(VEC2(1), _size);
  _size[0] /= (float)w;
  _size[1] /= (float)h;
  vec2_mul_f(_size, 2, _size);
  shader_use(&text_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g->tex); 
	shader_set_int(&text_shader, "tex", 0);
  shader_set_vec3(&text_shader, "tint", VEC3(1));
  shader_set_vec2(&text_shader, "pos", _pos);
  shader_set_vec2(&text_shader, "size", _size);
		
  glBindVertexArray(g->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}
void text_draw_glyph_box(vec2 pos, glyph* g, rgbf color)
{
  window_get_size(&w, &h);
  
  vec2 _pos;
  vec2_copy(pos, _pos);
  _pos[0] -= w; // * 0.5f;
  _pos[1] += h; // * 0.5f;
  _pos[0] *= 0.5f;
  _pos[1] *= 0.5f;
  vec2 _size;
  vec2_copy(VEC2(1), _size);
  _size[0] /= w;
  _size[1] /= h;
  vec2_mul_f(_size, 2, _size); 
  
  shader_use(&text_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blank_tex); 
	shader_set_int(&text_shader, "tex", 0);
  shader_set_vec3(&text_shader, "tint", color);
  shader_set_vec2(&text_shader, "pos", _pos);
  shader_set_vec2(&text_shader, "size", _size);

	glBindVertexArray(g->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}
void text_draw_img(vec2 pos, vec2 size, u32 tex, rgbf tint)
{
  window_get_size(&w, &h);
 
  // project: pixel -> ndc
  vec2 _pos;
  vec2_copy(pos, _pos);
  // ============================
  //           von erik
  // ============================
  _pos[0] = 2 * (_pos[0] / w) -1;
  _pos[1] = 2 * (_pos[1] / h) +1;

  vec2 _size;
  vec2_copy(VEC2(1), _size);
  _size[0] /= w;
  _size[1] /= h;
  // vec2_mul_f(_size, 2, _size);
  vec2_mul(_size, size, _size);
  
  // P_IL_VEC2(pos); PF("  >>  "); P_VEC2(_pos);
  _pos[0] += _size[0];
  // _pos[1] -= _size[1];

  shader_use(&img_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex); 
	shader_set_int(&img_shader, "tex", 0);
  shader_set_vec3(&img_shader, "tint", tint);
  shader_set_vec2(&img_shader, "pos", _pos);
  shader_set_vec2(&img_shader, "size", _size);

	glBindVertexArray(img_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
  
  // debug
  static bool is_debug = false;
  if (!is_debug) 
  {
    is_debug = true;
    text_draw_img(_pos, VEC2(10), blank_tex, RGB_F(0.8f, 0.1f, 0.4f));
    is_debug = false;
  }

}
void text_draw_quad(vec2 pos, vec2 size, rgbf color)
{
  text_draw_img(pos, size, blank_tex, color);
}

// ---- bulk draw ----

void text_draw_line(vec2 pos, glyph** g, int g_len)
{
  window_get_size(&w, &h);
  int g_w, g_h;
  text_get_glyph_size_estim(&g_w, &g_h);
 
  for(int i = 0; i < g_len; ++i)
  {
    // just for debug
    if(pos[0] + (g_w*2) >= w * 2)
    { pos[1] -= g_h; pos[0] = 0; }
     
    if (glyph_box_act)
    { text_draw_glyph_box(pos, g[i], VEC3_XYZ(1, 0, 1)); }
     
    text_draw_glyph(pos, g[i]); 
    pos[0] += g[i]->advance;
  }
}
void text_line_pos(int _g, vec2 pos, glyph** g, int g_len)
{
  window_get_size(&w, &h);
  int g_w, g_h;
  text_get_glyph_size_estim(&g_w, &g_h);
  for(int i = 0; i < g_len; ++i)
  {
    // just for debug
    if(pos[0] + (g_w*2) >= w * 2)
    { pos[1] -= g_h; pos[0] = 0; }
     
    if (i == _g) { break; }
    pos[0] += g[i]->advance;
  }
}


void text_draw_screen(glyph** g, int g_len)
{
  window_get_size(&w, &h);
  int g_w, g_h;
  text_get_glyph_size_estim(&g_w, &g_h);
 
  vec2 pos  = VEC2_Y_INIT(-g_h); // window bar on top
  // vec2 size = VEC2_INIT(0.001f);
  for(int i = 0; i < g_len; ++i)
  {
    if(g[i]->code == U_EOF || g[i]->code == U_NULL)
    { break; }

    if(g[i]->code == U_CR)  // 0x0D: '\n', carriage return
    { 
      // text_draw_glyph_box(pos, g_full, RGB_F(0, 1, 1));
      pos[1] -= g_h; pos[0] = 0; continue; 
    }
    
    if(pos[0] + (g_w*2) >= w * 2)
    { pos[1] -= g_h; pos[0] = 0; }
    
    if (glyph_box_act)
    { text_draw_glyph_box(pos, g[i], VEC3_XYZ(1, 0, 1)); }
    
    text_draw_glyph(pos, g[i]); 
    pos[0] += g[i]->advance;
  }
}
void text_screen_pos(int _g, vec2 pos, glyph** g, int g_len)
{
  // @TODO:
  window_get_size(&w, &h);
  int g_w, g_h;
  text_get_glyph_size_estim(&g_w, &g_h);
 
  vec2_copy(VEC2_Y(-g_h), pos); // window bar on top
  // vec2 size = VEC2_INIT(0.001f);
  for(int i = 0; i < g_len; ++i)
  {
    if(g[i]->code == U_EOF || g[i]->code == U_NULL)
    { break; }
    
    if(pos[0] + (g_w*2) >= w * 2)
    { pos[1] -= g_h; pos[0] = 0; }
    
    if (i == _g) { break; }
    
    pos[0] += g[i]->advance;
    
    if(g[i]->code == U_CR)  // 0x0D: '\n', carriage return
    { pos[1] -= g_h; pos[0] = 0; }
  }
}

