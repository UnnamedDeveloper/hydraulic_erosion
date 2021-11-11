#version 330 core

in vec3 v_color;
in vec3 v_normal;
in vec3 v_frag_pos;

out vec4 o_color;

uniform vec3 u_light_pos;
uniform vec3 u_camera_pos;

void main()
{
	vec3 light_color = vec3(0.2, 0.2, 0.2);

	float ambient_strength = 0.2;
	vec3 ambient = vec3(ambient_strength) * light_color;

	vec3 norm = normalize(v_normal);
	vec3 light_dir = normalize(u_light_pos - v_frag_pos);

	float diffuse_strength = 5.0;
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diffuse_strength * diff * light_color;

	float specular_strength = 0.5;
	vec3 view_dir = normalize(u_camera_pos - v_frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
	vec3 specular = specular_strength * spec * light_color;

	o_color = vec4((ambient + diffuse + specular) * v_color, 1.0);
}
