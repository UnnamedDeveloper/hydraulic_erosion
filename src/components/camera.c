#include "camera.h"

void camera_update_projection(camera_t *camera, uvec2 size)
{
	glm_perspective(glm_rad(70.0f), (float)size.x / (float)size.y, 0.1f, 100.0f, camera->projection);
}

void camera_create_view_matrix(camera_t *camera, mat4 view)
{
	glm_mat4_identity(view);
	glm_translate(view, (vec3) { 0.0f, 0.0f, -3.0f });
	return view;
}
