#version 460 core

layout (location = 0) in vec3 aPosition;    // the position is in attrib-index '0'
layout (location = 1) in vec2 aTexCoords;    // the uv-coordinates in attrib-index '2'


out VS_OUT
{
  vec2 tex_coords;
  vec3 frag_pos;
} _out;

uniform mat4 proj;          // matrix for camera projection
uniform mat4 view;          // matrix for view transformation
uniform mat4 model;         // matrix for applying the objects transform

uniform vec2 pos;
uniform vec2 scale;

void main() 
{

  // get pos from uniforms
  // vec2 _pos = pos + aPosition.xy;
  // _pos *= scale;

  _out.frag_pos = vec3(model * vec4(aPosition.xyz, 1.0));
  // _out.frag_pos = vec3(_pos.xy, 1.0);
  _out.tex_coords = aTexCoords;

  // the MVP matrix model view projection
  // don't change the order in the multiplication!
  // matrices are multiplied in reverse
  // gl_Position = proj * view * model * vec4(aPosition.xyz, 1);
  // gl_Position = vec4(_pos.xy, 1.0, 1.0);
  gl_Position =  proj * view * model * vec4(aPosition.xyz, 1.0);
}
