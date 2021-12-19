#ifndef SHADER_H
#define SHADER_H

#include "core.h"
#include "../math/math_inc.h"

typedef struct shader
{
	u32 handle;
	char* name;
	bool has_error;
}shader;


// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 create_shader(const char* vert_shader_src, const char* frag_shader_src, const char* name, bool* has_error);

// generate a shader-program from a vertex- and fragment-shader
// give the filepath to the vert / frag shader as the arguments
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
shader create_shader_from_file(const char* vert_path, const char* frag_path, const char* name);


// activate / use the shader 
void shader_use(shader* s);

// "free's" the shader program
// !!! only deletes the opngl handle not the name, etc.
void shader_delete(shader* s);

// set a bool in the shader 
// the given int is used as the bool ( 0/1 )
void shader_set_bool(shader* s, const char* name, int value);
// set an integer in the shader
void shader_set_int(shader* s, const char* name, int value);
// set a float in the shader
void shader_set_float(shader* s, const char* name, f32 value);
// set a vec2 in the shader
void shader_set_vec2_f(shader* s, const char* name, f32 x, f32 y);
// set a vec2 in the shader
void shader_set_vec2(shader* s, const char* name, vec2 v);
// set a vec3 in the shader
void shader_set_vec3_f(shader* s, const char* name, f32 x, f32 y, f32 z);
// set a vec3 in the shader
void shader_set_vec3(shader* s, const char* name, vec3 v);
// set a matrix 4x4 in the shader
void shader_set_mat4(shader* s, const char* name, mat4 value);

#endif
