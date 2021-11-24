#version 410 core

layout(location = 0) in vec3 i_pos;
layout(location = 1) in vec3 i_normal;

out vec3 v_normal;
out vec3 v_frag_pos;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
	gl_Position = u_projection * u_view * u_model * vec4(i_pos, 1.0);
	v_normal = i_normal;
	v_frag_pos = mat3(transpose(inverse(u_model))) * v_normal;
}
