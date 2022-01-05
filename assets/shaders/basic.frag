#version 460 core

out vec4 FragColor;

//passed from vertex-shader
in VS_OUT
{
  vec2 tex_coords;
  vec3 frag_pos;
} _in;

uniform vec3 tint;
uniform sampler2D tex;

void main()
{
  FragColor = texture(tex, _in.tex_coords) * vec4(tint.rgb, 1.0);
}
