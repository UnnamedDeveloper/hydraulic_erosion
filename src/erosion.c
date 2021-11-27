#include "erosion.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include <cglm/cglm.h>

#include "debug/assert.h"

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
	float random = ((float) rand() / (float) RAND_MAX);
	return random * (terrain_get_size(terrain).w - 1.1f);
}

static float rand_y_position_on_terrain(terrain_t *terrain)
{
	float random = ((float) rand() / (float) RAND_MAX);
	return random * (terrain_get_size(terrain).h - 1.1f);
}

static float height_at(terrain_t *t, int x, int y)
{
	return terrain_get_height(t, (uint32_t)x, (uint32_t)y);
}

static float set_height_at(terrain_t *t, int x, int y, float v)
{
	terrain_set_height(t, (uint32_t)x, (uint32_t)y, v);
}

static float get_drop_height(terrain_t *t, vec2 pos)
{
	int ix = (int) pos[0];
	int iz = (int) pos[1];

	float u = pos[0] - ix;
	float v = pos[1] - iz;

	return height_at(t, ix,     iz    ) * (1 - u) * (1 - v) +
		   height_at(t, ix + 1, iz    ) * u       * (1 - v) +
		   height_at(t, ix,     iz + 1) * (1 - u) * v +
		   height_at(t, ix + 1, iz + 1) * u       * v;
}

static void deposit_terrain(terrain_t *t, vec2 pos, float amount)
{
	int ix = (int)pos[0];
	int iz = (int)pos[1];

	float u = pos[0] - ix;
	float v = pos[1] - iz;

	float cells[2][2] = {
		{ height_at(t, ix, iz    ), height_at(t, ix + 1, iz    ) },
		{ height_at(t, ix, iz + 1), height_at(t, ix + 1, iz + 1) },
	};

	set_height_at(t, ix,     iz    , cells[0][0] + (amount * (1 - u) * (1 - v)));
	set_height_at(t, ix + 1, iz    , cells[0][1] + (amount * u       * (1 - v)));
	set_height_at(t, ix,     iz + 1, cells[1][0] + (amount * (1 - u) * v      ));
	set_height_at(t, ix + 1, iz + 1, cells[1][1] + (amount * u       * v      ));
}

static float erode_terrain(terrain_t *t, vec2 pos, int radius, float amount)
{
	int ix = (int)pos[0];
	int iz = (int)pos[1];

	float weight_sum = 0;
	float *weights = malloc(((radius + 1) * 2) * ((radius + 1) * 2) * sizeof(float));

	// calculate relevant weights for the erosion
	int i = 0;
	for (int x = -radius; x <= radius; x++)
	{
		int coord_x = ix + x;
		if (coord_x < 0 || coord_x >= terrain_get_size(t).w) continue;

		for (int z = -radius; z <= radius; z++)
		{
			int coord_z = iz + z;
			if (coord_z < 0 || coord_z >= terrain_get_size(t).h) continue;

			// calculate the weight based on the distance from the erosion center
			float distance = glm_vec2_distance(pos, (vec2) { coord_x + 0.5f, coord_z + 0.5f });
			float weight = fmax(radius - distance, 0);
			weights[i] = weight;
			weight_sum += weight;
			i++;
		}
	}

	float eroded = 0;

	// erode the requested amount based on the calculated weights
	i = 0;
	for (int x = -radius; x <= radius; x++)
	{
		int coord_x = ix + x;
		if (coord_x < 0 || coord_x >= terrain_get_size(t).w) continue;

		for (int z = -radius; z <= radius; z++)
		{
			int coord_z = iz + z;
			if (coord_z < 0 || coord_z >= terrain_get_size(t).h) continue;

			// calculate the exact value to erode the current point
			float h = height_at(t, coord_x, coord_z);
			float we = amount * (weights[i] / weight_sum);
			float erode = h > we ? we : h;

			// erode
			set_height_at(t, coord_x, coord_z, h - erode);
			eroded += erode;
			i++;
		}
	}

	free(weights);

	return eroded;
}

void hydraulic_erosion(terrain_t *terrain, const erosion_desc_t *params)
{
	// create a drop on a random position on the terrain
	drop_t drop = (drop_t) {
		.pos = { rand_x_position_on_terrain(terrain), rand_y_position_on_terrain(terrain) },
		.water = 1.0f,
		.velocity = 1.0f,
	};

	for (int iteration = 0; iteration < params->drop_lifetime; iteration++)
	{
		int ix = (int)drop.pos[0];
		int iz = (int)drop.pos[1];

		float u = drop.pos[0] - ix;
		float v = drop.pos[1] - iz;

		float neighbors[2][2] = {
			{ height_at(terrain, ix, iz    ), height_at(terrain, ix + 1, iz    ) },
			{ height_at(terrain, ix, iz + 1), height_at(terrain, ix + 1, iz + 1) },
		};

		// calculate the gradient of the slope the drop is currently on
		vec2 gradient = GLM_VEC2_ZERO_INIT;
		gradient[0] = ((neighbors[0][1] - neighbors[0][0]) * (1 - v)) + ((neighbors[1][1] - neighbors[1][0]) * v);
		gradient[1] = ((neighbors[1][0] - neighbors[0][0]) * (1 - u)) + ((neighbors[1][1] - neighbors[0][1]) * u);

		// calculate the direction on the drop based on its current direction as well as the slope
		drop.direction[0] = (drop.direction[0] * params->inertia) - (gradient[0] * (1 - params->inertia));
		drop.direction[1] = (drop.direction[1] * params->inertia) - (gradient[1] * (1 - params->inertia));
		glm_vec2_normalize(drop.direction);

		// move the drop
		vec2 old_pos; glm_vec2_copy(drop.pos, old_pos);
		glm_vec2_add(drop.pos, drop.direction, drop.pos);

		// kill the drop if it has left the map bounds or stopped
		if (drop.pos[0] < 0 || drop.pos[0] >= terrain_get_size(terrain).w - 1 ||
			drop.pos[1] < 0 || drop.pos[1] >= terrain_get_size(terrain).h - 1 ||
			(drop.direction[0] == 0 && drop.direction[1] == 0)) break;

		// find the height difference between the last and current position
		float height_dif = get_drop_height(terrain, drop.pos) - get_drop_height(terrain, old_pos);

		// calculate the capacity of the droplet based on its speed, water content and the capacity modifier
		float capacity = fmax((-height_dif) * drop.velocity * drop.water * params->capacity, params->min_capacity);
		HE_ASSERT(!isnan(capacity), "Failed to calculate capacity");

		if (height_dif > 0)
		{
			// try to equal height
			float deposit = fmin(drop.sediment, height_dif);
			drop.sediment -= deposit;
			deposit_terrain(terrain, old_pos, deposit);
		}
		else if (drop.sediment > capacity)
		{
			// deposit sediment
			float deposit = (drop.sediment - capacity) * params->deposition;
			drop.sediment -= deposit;
			deposit_terrain(terrain, old_pos, deposit);
		}
		else
		{
			// erode terrain
			float erode = fmin((capacity - drop.sediment) * params->erosion, -height_dif);
			drop.sediment += erode_terrain(terrain, old_pos, params->radius, erode);
		}

		// update drop velocity and water content
		drop.velocity = sqrt(drop.velocity * drop.velocity + height_dif * params->gravity);
		drop.water *= (1 - params->evaporation);
	}
}
