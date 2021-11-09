#ifndef __components_camera_h__
#define __components_camera_h__

#include <cglm/cglm.h>

#include "math/types.h"

typedef struct camera_t
{
	mat4 projection;
} camera_t;

void camera_update_projection(camera_t *camera, uvec2 size);

#endif /* __components_camera_h__ */
