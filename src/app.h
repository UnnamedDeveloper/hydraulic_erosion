#ifndef __app_h__
#define __app_h__

#include <stdbool.h>

#include <cglm/cglm.h>

#include "components/camera.h"
#include "components/terrain.h"
#include "events/event.h"
#include "gfx/window.h"
#include "imgui/imgui_context.h"

#define APP_NAME "Hydraulic Erosion"

typedef struct app_state_t
{
	bool running;

	window_t *window;
	event_bus_t *event_bus;
	imgui_context_t *imgui;

	terrain_t *terrain;

	camera_t *camera;
} app_state_t;

bool app_init(app_state_t *state);
void app_run(app_state_t *state);
void app_shutdown(app_state_t *state);

#endif /* __app_h__ */
