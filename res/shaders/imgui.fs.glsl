#version 410 core

in vec2 v_uv;
in vec4 v_col;

out vec4 o_col;

uniform sampler2D tex;

void main()
{
	o_col = v_col * texture(tex, v_uv);
}
