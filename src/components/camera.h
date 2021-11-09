#ifndef __components_camera_h__
#define __components_camera_h__

#include <cglm/cglm.h>

#include "math/types.h"

typedef struct camera_t
{
	vec3 target;
	vec2 angle;
	float distance;

	float fov;

	mat4 projection;
	vec3 position;
} camera_t;

void camera_init(float fov, float distance, uvec2 size, camera_t *camera);

void camera_update_projection(camera_t *camera, uvec2 size);
void camera_create_view_matrix(camera_t *camera, mat4 view);
void camera_move(camera_t *camera, float distance_offset, vec2 angle_offset);
void camera_set_target(camera_t *camera, vec2 target);

#endif /* __components_camera_h__ */
