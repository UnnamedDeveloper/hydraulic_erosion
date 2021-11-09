#ifndef __app_h__
#define __app_h__

#include <stdbool.h>

#include <cglm/cglm.h>

#include "components/camera.h"
#include "events/event.h"
#include "gfx/mesh.h"
#include "gfx/pipeline.h"
#include "gfx/window.h"

#define APP_NAME "Hydraulic Erosion"

typedef struct app_state_t
{
	bool running;

	window_t *window;
	event_bus_t *event_bus;

	struct
	{
		mesh_t *mesh;
		pipeline_t *pipeline;
	} terrain;

	camera_t camera;
} app_state_t;

bool app_init(app_state_t *state);
void app_run(app_state_t *state);
void app_shutdown(app_state_t *state);

#endif /* __app_h__ */
