#ifndef __app_h__
#define __app_h__

#include <stdbool.h>

#include "events/event.h"
#include "gfx/window.h"

#define APP_NAME "Hydraulic Erosion"

typedef struct app_state_t
{
	bool running;
	window_t *window;
	event_bus_t *event_bus;
} app_state_t;

bool app_init(app_state_t *state);
void app_run(app_state_t *state);
void app_shutdown(app_state_t *state);

#endif /* __app_h__ */
