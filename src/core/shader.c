#include "shader.h"
#include <stdio.h>
#include <GLAD/glad.h>


// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
u32 create_shader(const char* vert_shader_src, const char* frag_shader_src, const char* name, bool* has_error)
{
	// build and compile our shader program
	// ------------------------------------
	
	*has_error = false;

	// vertex shader
	u32 vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vert_shader_src, NULL);
	glCompileShader(vertexShader);

	// check for shader compile errors
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "%s-!!!-> ERROR_VERTEX_COMPILATION: [%s]\n -> %s\n", vert_shader_src, name, infoLog);
		*has_error = true;
	}

	// fragment shader
	u32 fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &frag_shader_src, NULL);
	glCompileShader(fragmentShader);

	// check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "%s\n-!!!-> ERROR_FRAGMENT_COMPILATION: [%s]\n -> %s\n", frag_shader_src, name, infoLog);
		*has_error = true;
	}

	// link shaders
	u32 shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fprintf(stderr, "-!!!-> ERROR_PROGRAM_LINKING: \n -> %s\n", infoLog);
		*has_error = true;
	}

	// free the shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}


// generate a shader-program from a vertex- and fragment-shader
// returns: a pointer to the opengl shader program as a "unsigned int" aka. "u32"
shader create_shader_from_file(const char* vert_path, const char* frag_path, const char* name) // const char* vert_path, const char* frag_path
{
	// read the shader from file
	// ------------------------------------
	// const char* vert_shader_src_read = read_text_file(vert_path);
	// const char* frag_shader_src_read = read_text_file(frag_path);

	// ---- vert ---- 

	FILE* f;
	char* vert_src;
	u64 len;

	f = fopen(vert_path, "rb");
	ERR_CHECK(f != NULL, "loading vert shader text-file at: %s\n", vert_path);
	

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f); 
	assert(len > 0);
	fseek(f, 0, SEEK_SET);
  len++; // null-terminator

	// alloc memory 
	vert_src = calloc(1, len);
	assert(vert_src != NULL);

	// fill text buffer
	fread(vert_src, sizeof(char), len, f);
	assert(strlen(vert_src) > 0);
	fclose(f);
  vert_src[len -1] = '\0';

	// --------------

	// ---- frag ----

	char* frag_src;
	len = 0;

	f = fopen(frag_path, "rb");
	if (f == NULL) {
		fprintf(stderr, "error loading frag shader text-file at: %s\n", frag_path);
		assert(false);
	}

	// get len of file
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	assert(len > 0);
	fseek(f, 0, SEEK_SET);
  len++; // null-terminator

	// alloc memory 
	frag_src = calloc(1, len);
	assert(frag_src != NULL);

	// fill text buffer
	fread(frag_src, sizeof(char), len, f);
	assert(strlen(frag_src) > 0);
	fclose(f);
  frag_src[len -1] = '\0';

	// --------------

	bool has_error = false;
	u32 handle = create_shader(vert_src, frag_src, name, &has_error);

	shader s;
	s.handle = handle;
	s.name = (char*)name;
	s.has_error = has_error;

	// remember to free the memory allocated by read_text_file()
	free(vert_src);
	free(frag_src);

	return s;
}

// activate / use the shader 
void shader_use(shader* s)
{
	glUseProgram(s->handle);
}
void shader_delete(shader* s)
{
	glDeleteProgram(s->handle);
}

// set a bool in the shader 
// the given int is used as the bool ( 0/1 )
void shader_set_bool(shader* s, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(s->handle, name), value);
}
// set an integer in the shader
void shader_set_int(shader* s, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(s->handle, name), value);
}
// set a float in the shader
void shader_set_float(shader* s, const char* name, f32 value)
{
	glUniform1f(glGetUniformLocation(s->handle, name), value);
}
// set a vec2 in the shader
void shader_set_vec2_f(shader* s, const char* name, f32 x, f32 y)
{
	glUniform2f(glGetUniformLocation(s->handle, name), x, y);
}
// set a vec2 in the shader
void shader_set_vec2(shader* s, const char* name, vec2 v)
{
	glUniform2f(glGetUniformLocation(s->handle, name), v[0], v[1]);
}
// set a vec3 in the shader
void shader_set_vec3_f(shader* s, const char* name, f32 x, f32 y, f32 z)
{
	glUniform3f(glGetUniformLocation(s->handle, name), x, y, z);
}
// set a vec3 in the shader
void shader_set_vec3(shader* s, const char* name, vec3 v)
{
	glUniform3f(glGetUniformLocation(s->handle, name), v[0], v[1], v[2]);
}
// set a matrix 4x4 in the shader
void shader_set_mat4(shader* s, const char* name, mat4 value)
{
	u32 transformLoc = glGetUniformLocation(s->handle, name);
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, value[0]);
}
