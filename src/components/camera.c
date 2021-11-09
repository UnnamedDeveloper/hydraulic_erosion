#include "camera.h"

#include <math.h>

static void update_camera_position(camera_t *camera)
{
	float horizontal_dist = camera->distance * cos(glm_rad(camera->angle[1]));
	float vertical_dist   = camera->distance * sin(glm_rad(camera->angle[1]));

	float theta = glm_rad(camera->angle[0]);
	camera->position[0] = camera->target[0] - (horizontal_dist * sin(theta));
	camera->position[2] = camera->target[2] - (horizontal_dist * cos(theta));
	camera->position[1] = camera->target[1] - vertical_dist;
}

void camera_init(float fov, float distance, uvec2 size, camera_t *camera)
{
	glm_vec3_zero(camera->target);
	glm_vec3_zero(camera->position);
	glm_vec2_zero(camera->angle);
	camera->fov = fov;
	camera->distance = distance;
	camera_update_projection(camera, size);
	update_camera_position(camera);
}

void camera_update_projection(camera_t *camera, uvec2 size)
{
	glm_perspective(glm_rad(camera->fov), (float)size.x / (float)size.y, 0.1f, 100.0f, camera->projection);
}

void camera_create_view_matrix(camera_t *camera, mat4 view)
{
	glm_mat4_identity(view);
	glm_lookat(camera->position, camera->target, (vec3){ 0.0f, 1.0f, 0.0f }, view);
}

void camera_move(camera_t *camera, float distance_offset, vec2 angle_offset)
{
	camera->distance += distance_offset;
	glm_vec2_add(camera->angle, angle_offset, camera->angle);

	update_camera_position(camera);
}
