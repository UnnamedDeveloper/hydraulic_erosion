#ifndef __components_camera_h__
#define __components_camera_h__

#include <cglm/cglm.h>

#include "math/types.h"

typedef struct camera_t
{
	mat4 projection;
} camera_t;

void camera_update_projection(camera_t *camera, uvec2 size);
void camera_create_view_matrix(camera_t *camera, mat4 view);

#endif /* __components_camera_h__ */
