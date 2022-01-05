#version 460 core

layout (location = 0) in vec3 aPosition;    // the position is in attrib-index '0'
layout (location = 1) in vec2 aTexCoords;    // the uv-coordinates in attrib-index '2'
out VS_OUT
{
  vec2 tex_coords;
  vec3 frag_pos;
} _out;

uniform vec2 pos;
uniform vec2 size;

void main() 
{
  _out.frag_pos = vec3(vec4(aPosition.xy, 0.0, 1.0));
  _out.tex_coords = aTexCoords;

  gl_Position = vec4((aPosition.xy + pos) * size, 0.0, 1.0);
}


