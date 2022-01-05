#version 460 core

layout (location = 0) in vec2 aPosition;    // the position is in attrib-index '0'


out VS_OUT
{
  vec2 tex_coords;
  vec3 frag_pos;
} _out;

uniform mat4 view;	
uniform mat4 proj;

void main() 
{
  _out.frag_pos = vec3(aPosition.xy, 0.0);
  _out.tex_coords = vec2(0.0);

  // the MVP matrix model view projection
  // don't change the order in the multiplication!
  // matrices are multiplied in reverse
  // gl_Position = proj * view * model * vec4(aPosition.xyz, 1);
  gl_Position = proj * view * vec4(aPosition.xy, 0.0, 1.0);
}
