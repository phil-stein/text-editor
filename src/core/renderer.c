#include "renderer.h"
#include "window.h"
#include "shader.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLAD/glad.h>


// ---- vars ----
int w = 0;  // screen width
int h = 0;  // screen height

bool wireframe_mode_enabled = false;

// vao, vbo for the quad used in render_quad()
u32 quad_vao, quad_vbo;
shader quad_shader;
texture blank_tex;

// debug lines
#ifdef DEBUG
shader line_shader;
u32 line_vao, line_vbo;
#define DEBUG_LINES_MAX 100
d_line debug_lines[DEBUG_LINES_MAX];
int debug_lines_len = 0;
#endif

// cam
const f32 cam_fov = 45.0f;
vec2 cam_pos = VEC2_INIT(0.0f);
const f32 cam_n_plane = 0.1f;
const f32 cam_f_plane = 100.f;


// tmp
vec2 color_pos  = VEC2_XY_INIT(-2, -3);
vec2 color_size = VEC2_INIT(0.5f);
vec3 color_col  = VEC3_XYZ_INIT(1, 0, 1);
vec2 crate_pos  = VEC2_XY_INIT(-3, -3);
vec2 crate_size = VEC2_INIT(0.5f);
texture crate_tex;
shader test_shader;

void renderer_init()
{

	// ---- quad -----
	f32 quad_verts[] = 
	{ 
	// positions   // tex coords
	-1.0f,  1.0f,  0.0f, 1.0f,
	-1.0f, -1.0f,  0.0f, 0.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,

	-1.0f,  1.0f,  0.0f, 1.0f,
	 1.0f, -1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f,  1.0f, 1.0f
	};

	// screen quad VAO
	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quad_vbo);
	glBindVertexArray(quad_vao);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(f32), &quad_verts, GL_STATIC_DRAW); // quad_verts is 24 long
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(f32), (void*)(2 * sizeof(f32)));

	quad_shader = create_shader_from_file("assets/shaders/basic.vert", 
					      "assets/shaders/basic.frag", 
					      "quad_shader");

  blank_tex = texture_create_from_path("assets/textures/blank.png",
					    "blank", true); 
	
  crate_tex = texture_create_from_path("assets/textures/crate01.png",
					    "crate01", true); 
 
  #ifdef DEBUG
  // ---- line vao & vbo ----
	f32 line_verts[] = 
	{
		0, 0,
		0, 1,
		0, 0
	};	
	glGenVertexArrays(1, &line_vao);
	glGenBuffers(1, &line_vbo);
	glBindVertexArray(line_vao);
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
	// verts is 4 long
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(f32), &line_verts, GL_STATIC_DRAW); 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(f32), (void*)0);
	// unbind vao
	glBindVertexArray(0);

	line_shader = create_shader_from_file("assets/shaders/line_text.vert", 
					      "assets/shaders/basic.frag", 
					      "line_shader");
		
  test_shader = create_shader_from_file("assets/shaders/test.vert", 
					      "assets/shaders/text.frag", 
					      "test_shader");
  #endif

  // ---- state ----
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

}


void renderer_update()
{
	window_get_size(&w, &h);
	glViewport(0, 0, w, h);

	// ---- background ----
	f32 r = (f32)sin(glfwGetTime());
	f32 g = (f32)cos(glfwGetTime());
	f32 b = (f32)sin(glfwGetTime());
	r = (r + 1) * 0.5f;
	g = (g + 1) * 0.5f;
	b = (b + 1) * 0.5f;
	// glClearColor(r, g, b, 1.0f);
	// glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  // #282c34
  glClearColor(40.0f / 256.0f, 44.0f / 256.0f, 52.0f / 256.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
 
  // ---- rendering ----
  
	mat4 view;
	mat4_lookat_2d(cam_pos, 10.0f, view);

	mat4 proj;
	float pers = cam_fov;
	m_deg_to_rad(&pers);
	// int w, h;
	// get_window_size(&w, &h);
	mat4_perspective(pers, ((f32)w / (f32)h), cam_n_plane, cam_f_plane, proj);
  
  renderer_draw_quad(color_pos, color_size, color_col, view, proj);
	renderer_draw_quad_textured(crate_pos, crate_size, crate_tex, VEC3(1), view, proj);

  // draw_test(VEC2_XY(100, 100), VEC2(1));
 
  #ifdef DEBUG
  // render debug_lines and reset
	for (int i = 0; i < debug_lines_len; ++i)
	{
		d_line* l = &debug_lines[i];
		renderer_draw_line(l->p0, l->p1, l->color, view, proj);
	}
	debug_lines_len = 0;
  #endif

}

void renderer_draw_quad(vec2 pos, vec2 size, vec3 color, mat4 view, mat4 proj)
{
  renderer_draw_quad_textured(pos, size, blank_tex, color, view, proj);
}
void renderer_draw_quad_textured(vec2 pos, vec2 size, texture tex, vec3 tint, mat4 view, mat4 proj)
{
	// ---- mvp ----
	
	mat4 model;
	mat4_make_model_2d(pos, size, 0.0f, model);

	// ---- shader & draw call -----	

	shader_use(&quad_shader);
	shader_set_vec3(&quad_shader, "tint", tint);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wireframe_mode_enabled ? blank_tex.handle : tex.handle); 
	shader_set_int(&quad_shader, "tex", 0);
	
	shader_set_mat4(&quad_shader, "model", model);
	shader_set_mat4(&quad_shader, "view", view);
	shader_set_mat4(&quad_shader, "proj", proj);

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}

#ifdef DEBUG
void renderer_draw_line(vec2 p0, vec2 p1, vec3 color, mat4 view, mat4 proj)
{
	// ---- vao & vbo ----
	f32 verts[] = 
	{
		p0[0], p0[1],
		p1[0], p1[1],
		p0[0], p0[1]
	};	
	glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(f32), verts);

	// ---- shader & draw call -----	
	shader_use(&line_shader);
	shader_set_vec3(&line_shader, "tint", color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blank_tex.handle); 
	shader_set_int(&line_shader, "tex", 0);
	shader_set_mat4(&line_shader, "view", view);
	shader_set_mat4(&line_shader, "proj", proj);

	glLineWidth(10.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBindVertexArray(line_vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);
	if (!wireframe_mode_enabled)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glLineWidth(1.0f);
}
#endif

void draw_test(vec2 pos, vec2 size)
{
  static bool g_init = false;
  static glyph* g;
  if (!g_init) { g = text_get_glyph('W'); }

  window_get_size(&w, &h);
	mat4 proj;
  // mat4_ortho_clip(0, w, h, 0, -0.01f, 100, proj); 
  // mat4_ortho_clip(float left, float right, float bottom,  float top, float nearVal, float farVal, mat4  dest) 
  mat4_ortho(0, w, 0, h, proj);
  // mat4_ortho(-1.0f, 1.0f, -1.0f, 1.0f, proj);
  // mat4_ortho(1.0f, 0.0f, 1.0f, 0.0f, proj);
  
  // ---- shader & draw call -----	
	shader_use(&test_shader);
  glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blank_tex.handle); 
	shader_set_int(&line_shader, "tex", 0);
  shader_set_vec3(&test_shader, "tint", VEC3_XYZ(1, 0, 1));
  shader_set_vec2(&test_shader, "pos", pos);
  shader_set_vec2(&test_shader, "size", size);
  shader_set_mat4(&test_shader, "proj", proj);
		
  glBindVertexArray(g->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}
// ---- renderer direct api ----

void renderer_submit_line_colored(vec2 p0, vec2 p1, vec3 color)
{
  #ifdef DEBUG
	if (debug_lines_len >= DEBUG_LINES_MAX -1) { return; }
	
	d_line l;
	vec2_copy(p0, l.p0);
	vec2_copy(p1, l.p1);
	vec3_copy(color, l.color);

	debug_lines[debug_lines_len] = l;
	debug_lines_len++;
  #endif
}


// ---- getter & setter ----

void renderer_set_wireframe(bool act)
{
	wireframe_mode_enabled = act;

	// draw in solid-mode for fbo
	if (wireframe_mode_enabled == true)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}


}

void renderer_set_cam_pos(vec3 pos)
{
	vec3_copy(pos, cam_pos);
}
void renderer_get_cam_pos(vec3 pos)
{
	vec3_copy(cam_pos, pos);
}
void renderer_move_cam(vec3 dist)
{
	vec3_add(cam_pos, dist, cam_pos);
}



