#ifndef RENDERER_H
#define RENDERER_H

#include "core.h"
#include "texture.h"
#include <math/math_inc.h>
#include <text/text.h>


typedef struct debug_line
{
	vec2 p0;
	vec2 p1;
	vec3 color;
}d_line;

void renderer_init();
void renderer_update();

void renderer_draw_quad_textured(vec2 pos, vec2 size, texture tex, vec3 color, mat4 view, mat4 proj);
void renderer_draw_quad(vec2 pos, vec2 size, vec3 color, mat4 view, mat4 proj);
void draw_test(vec2 pos, vec2 size);

void renderer_draw_line(vec2 p0, vec2 p1, vec3 color, mat4 view, mat4 proj);
void renderer_submit_line_colored(vec2 p0, vec2 p1, vec3 color);
INLINE void renderer_submit_line(vec2 p0, vec2 p1)
{
	renderer_submit_line_colored(p0, p1, VEC3_XYZ(0.1f, 0.1f, 1.0f));
}
void renderer_set_wireframe(bool act);

void renderer_set_cam_pos(vec3 pos);
void renderer_get_cam_pos(vec3 pos);
void renderer_move_cam(vec3 dist);


#endif
