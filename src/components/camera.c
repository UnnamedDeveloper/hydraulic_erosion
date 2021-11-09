#include "camera.h"

void camera_update_projection(camera_t *camera, uvec2 size)
{
	glm_perspective(glm_rad(70.0f), (float)size.x / (float)size.y, 0.1f, 100.0f, camera->projection);
}
