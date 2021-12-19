#ifndef RENDERER_H
#define RENDERER_H

#include "core.h"
#include "texture.h"
#include "../math/math_inc.h"
#include "../text/text.h"


void renderer_init();
void renderer_update();

void renderer_draw_quad_textured(vec2 pos, vec2 size, texture tex, bool use_color, vec3 color);

void renderer_draw_quad(vec2 pos, vec2 size, vec3 color);


void renderer_set_wireframe(bool act);

void renderer_set_cam_pos(vec3 pos);
void renderer_get_cam_pos(vec3 pos);
void renderer_move_cam(vec3 dist);


#endif
