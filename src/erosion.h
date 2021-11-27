#ifndef __erosion_h__
#define __erosion_h__

#include "components/terrain.h"

typedef struct erosion_desc_t
{
	int drop_lifetime;
	float inertia;
	float capacity;
	float min_capacity;
	float deposition;
	float erosion;
	int radius;
	float gravity;
	float evaporation;
} erosion_desc_t;

#define EROSION_DEFAULT_DESC (erosion_desc_t) {\
	.drop_lifetime = 50,\
	.inertia = 0.05f,\
	.capacity = 4,\
	.min_capacity = 0.01f,\
	.deposition = 0.3f,\
	.erosion = 0.3f,\
	.radius = 3,\
	.gravity = 4.0f,\
	.evaporation = 0.05f,\
	}

void hydraulic_erosion(terrain_t *terrain, const erosion_desc_t *params);

#endif /* __erosion_h__ */
