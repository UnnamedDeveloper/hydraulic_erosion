#ifndef __components_camera_h__
#define __components_camera_h__

#include <cglm/cglm.h>

#include "gfx/window.h"
#include "math/types.h"

typedef struct camera_desc_t
{
	float fov;
	float distance;
	vec3 target;
	window_t *window;
} camera_desc_t;

typedef struct camera_t
{
	vec3 target;
	vec2 angle;
	float distance;

	float fov;

	mat4 projection;
	vec3 position;

	bool move;

	window_t *window;

} camera_t;

void camera_init(const camera_desc_t *desc, camera_t **camera);
camera_t *camera_create(const camera_desc_t *desc);
void camera_free(camera_t *camera);

void camera_update_projection(camera_t *camera);
void camera_create_view_matrix(camera_t *camera, mat4 view);
void camera_move(camera_t *camera, float distance_offset, vec2 angle_offset);
void camera_set_target(camera_t *camera, vec3 target);

#endif /* __components_camera_h__ */
