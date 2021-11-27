#ifndef __components_terrain_h__
#define __components_terrain_h__

#include <stdint.h>

#include <cglm/cglm.h>

#include "gfx/mesh.h"
#include "gfx/pipeline.h"
#include "math/types.h"
#include "camera.h"

typedef float(*terrain_noise_function_t)(int, float, float);
typedef struct terrain_t terrain_t;
typedef void(*terrain_erosion_function_t)(terrain_t *);

typedef struct terrain_desc_t
{
	vec3 position;
	uvec2 size;
	int seed;
	float scale_scalar;
	float elevation;
	terrain_noise_function_t noise_function;
} terrain_desc_t;

typedef struct terrain_t
{
	vec3 position;
	uvec2 size;

	int seed;
	float *height_map;
	float scale_scalar;
	float elevation;

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

void terrain_draw(camera_t *camera, vec3 light_pos, terrain_t *terrain);
void terrain_update_mesh(terrain_t *terrain);

void terrain_set_height(terrain_t *terrain, uint32_t x, uint32_t y, float v);
float terrain_get_height(terrain_t *terrain, uint32_t x, uint32_t y);
void terrain_resize(terrain_t *terrain, uvec2 size);
void terrain_reset(terrain_t *terrain);
uvec2 terrain_get_size(terrain_t *terrain);

#endif /* __components_terrain_h__ */
