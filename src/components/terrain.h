#ifndef __components_terrain_h__
#define __components_terrain_h__

#include <stdint.h>

#include <cglm/cglm.h>

#include "gfx/mesh.h"
#include "gfx/pipeline.h"
#include "math/types.h"
#include "camera.h"

typedef float(*terrain_noise_function_t)(uint32_t, uint32_t);

typedef struct terrain_desc_t
{
	vec3 position;
	uvec2 size;
	terrain_noise_function_t noise_function;
} terrain_desc_t;

typedef struct terrain_t
{
	vec3 position;
	uvec2 size;

	terrain_noise_function_t noise_function;

	mesh_t *mesh;
	pipeline_t *pipeline;
#ifndef NDEBUG
	pipeline_t *pipeline_wireframe;
#endif
} terrain_t;

void terrain_init(const terrain_desc_t *desc, terrain_t **terrain);
terrain_t *terrain_create(const terrain_desc_t *desc);
void terrain_free(terrain_t *terrain);

void terrain_draw(camera_t *camera, terrain_t *terrain);

void terrain_resize(terrain_t *terrain, uvec2 size);

#endif /* __components_terrain_h__ */
