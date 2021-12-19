#version 460 core

	out vec4 FragColor;
	
    	//passed from vertex-shader
    	in VS_OUT
    	{
        	vec2 tex_coords;
        	vec3 frag_pos;
    	} _in;
	
	uniform bool use_color;
	uniform vec3 color;
	uniform sampler2D tex;
	
	void main()
	{

		// FragColor = vec4(1.0, 1.0, 1.0, 1.0);
		// FragColor = vec4(_in.tex_coords.xy, 0.0, 1.0);
		if (use_color)
		{
			FragColor = vec4(color.rgb, 1.0);
		}
		else
		{
			FragColor = texture(tex, _in.tex_coords);
		}
	}
