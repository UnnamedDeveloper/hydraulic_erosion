#version 410 core

layout (location = 0) in vec2 i_pos;
layout (location = 1) in vec2 i_uv;
layout (location = 2) in vec4 i_col;

out vec2 v_uv;
out vec4 v_col;

uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * vec4(i_pos.xy, 0.0, 1.0);
	v_uv = i_uv;
	v_col = i_col;
}
