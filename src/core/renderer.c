#include "renderer.h"
#include "window.h"
#include "shader.h"

#define GLFW_INCLUDE_NONE
#include "../../external/GLFW/glfw3.h"
#include "../../external/GLAD/glad.h"


// ---- vars ----
int w = 0;  // screen width
int h = 0;  // screen height

bool wireframe_mode_enabled = false;

// vao, vbo for the quad used in render_quad()
u32 quad_vao, quad_vbo;
shader quad_shader;
texture blank_tex;


// cam
const f32 cam_fov = 45.0f;
vec2 cam_pos = VEC2_INIT(0.0f);
const f32 cam_n_plane = 0.1f;
const f32 cam_f_plane = 100.f;


// tmp
vec2 color_pos  = VEC2_XY_INIT(3, 1);
vec2 color_size = VEC2_INIT(0.5f);
vec3 color_col  = VEC3_XYZ_INIT(1, 0, 1);
vec2 crate_pos  = VEC2_XY_INIT(2, 1);
vec2 crate_size = VEC2_INIT(0.5f);
texture crate_tex;

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
	printf("quad_shader: name: '%s', handle: %d\n", quad_shader.name, quad_shader.handle);

  blank_tex = texture_create_from_path("assets/textures/blank.png",
					    "blank", true); 
	
  crate_tex = texture_create_from_path("assets/textures/crate01.png",
					    "crate01", true); 
 
  // ---- state ----
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);

}


void renderer_update()
{
	get_window_size(&w, &h);
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
	renderer_draw_quad(color_pos, color_size, color_col);
	renderer_draw_quad_textured(crate_pos, crate_size, crate_tex, false, VEC3(0));
}

void renderer_draw_quad(vec2 pos, vec2 size, vec3 color)
{
  renderer_draw_quad_textured(pos, size, blank_tex, true, color);
}
void renderer_draw_quad_textured(vec2 pos, vec2 size, texture tex, bool use_tint, vec3 tint)
{
	// ---- mvp ----
	
	mat4 model;
	mat4_make_model_2d(pos, size, 0.0f, model);

	mat4 view;
	mat4_lookat_2d(cam_pos, 10.0f, view);

	mat4 proj;
	float pers = cam_fov;
	m_deg_to_rad(&pers);
	// int w, h;
	// get_window_size(&w, &h);
	mat4_perspective(pers, ((f32)w / (f32)h), cam_n_plane, cam_f_plane, proj);

	// ---- shader & draw call -----	

	shader_use(&quad_shader);
	shader_set_int(&quad_shader, "use_color", use_tint);
	shader_set_vec3(&quad_shader, "color", tint);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wireframe_mode_enabled ? blank_tex.handle : tex.handle); 
	shader_set_int(&quad_shader, "tex", 0);
	
	shader_set_mat4(&quad_shader, "model", model);
	shader_set_mat4(&quad_shader, "view", view);
	shader_set_mat4(&quad_shader, "proj", proj);

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
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



