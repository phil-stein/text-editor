#include "text_draw.h"
#include "../core/window.h"
#include "../core/shader.h"
#include "../core/input.h"

#include "../../external/GLAD/glad.h"


static int w = 0;  // screen width
static int h = 0;  // screen height

shader text_shader;
unsigned int blank_tex = 0;

bool glyph_box_act = false;

void text_draw_init()
{
  text_shader = create_shader_from_file("assets/shaders/text.vert", 
					    "assets/shaders/text.frag", 
					    "text_shader");
	printf("text_shader: name: '%s', handle: %d\n", text_shader.name, text_shader.handle);
  
  blank_tex = texture_create_from_path("assets/textures/blank.png",
					    "blank", true).handle; 
 
  // ---- state ----
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
}

void text_draw_update()
{
  if (is_key_pressed(KEY_Enter))
  { glyph_box_act = !glyph_box_act; }
}

void text_draw_glyph(vec2 pos, vec2 size, glyph_info* g)
{
  get_window_size(&w, &h);
	
  // ---- shader & draw call -----	
	shader_use(&text_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g->tex); 
	shader_set_int(&text_shader, "tex", 0);
  shader_set_vec3(&text_shader, "tint", VEC3(1));
  
  vec2 _pos;
  vec2_copy(pos, _pos);
  _pos[0] -= w; // * 0.5f;
  _pos[1] += h; // * 0.5f;
  // vec2_copy(VEC2(0), _pos);
  _pos[0] /= w;
  _pos[1] /= h;
  shader_set_vec2(&text_shader, "pos", _pos);
  shader_set_vec2(&text_shader, "size", size);
		
  glBindVertexArray(g->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}
// @DEBUG:
void text_draw_glyph_box(vec2 pos, vec2 size, glyph_info* g, vec3 color)
{
  get_window_size(&w, &h);
  
  shader_use(&text_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blank_tex); 
	shader_set_int(&text_shader, "tex", 0);
  shader_set_vec3(&text_shader, "tint", color);
  
  vec2 _pos;
  vec2_copy(pos, _pos);
  _pos[0] -= w; // * 0.5f;
  _pos[1] += h; // * 0.5f;
  // vec2_copy(VEC2(0), _pos);
  _pos[0] /= w;
  _pos[1] /= h;
  shader_set_vec2(&text_shader, "pos", _pos);
  shader_set_vec2(&text_shader, "size", size);

	glBindVertexArray(g->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}

void text_draw_screen(glyph_info* g, int g_len)
{
  get_window_size(&w, &h);
  int g_w, g_h;
  text_get_glyph_size_estim(&g_w, &g_h);
 
  vec2 pos  = VEC2_Y_INIT(-35); // window bar on top
  // vec2 size = VEC2_INIT(0.001f);
  vec2 size = VEC2_INIT(1.0f);
  for(int i = 0; i < g_len; ++i)
  {
    if(g[i].code == 0x0D)  // 0x0D: '\n'
    { pos[1] -= g_h; pos[0] = 0; continue; }
    
    if(pos[0] + g_w >= w * 2)
    { pos[1] -= g_h; pos[0] = 0; }
    
    if (!glyph_box_act)
    { text_draw_glyph_box(pos, size, &g[i], VEC3_XYZ(1, 0, 1)); }
    
    text_draw_glyph(pos, size, &g[i]); 
    pos[0] += g[i].advance;
  }
}

