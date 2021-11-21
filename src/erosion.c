#include "erosion.h"

#include <stdlib.h>

#include <cglm/cglm.h>

typedef struct drop_t
{
	vec2 pos;
	vec2 direction;
	float velocity;
	float water;
	float sediment;
} drop_t;

static float rand_x_position_on_terrain(terrain_t *terrain)
{
	int x = rand() % terrain_get_size(terrain).w;
	return (float) x;
}

static float rand_y_position_on_terrain(terrain_t *terrain)
{
	int y = rand() % terrain_get_size(terrain).h;
	return (float) y;
}

void hydraulic_erosion(terrain_t *terrain)
{
	// create a drop on a random position on the terrain
	drop_t drop = (drop_t) {
		.pos = { rand_x_position_on_terrain(terrain), rand_y_position_on_terrain(terrain) },
	};

	// dig into current position
	float height = terrain_get_height(terrain, drop.pos[0], drop.pos[1]);
	terrain_set_height(terrain, drop.pos[0], drop.pos[1], height - 0.001f);
}
