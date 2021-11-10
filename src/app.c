#include "app.h"

#include <string.h>

#include "debug/assert.h"
#include "events/window_event.h"
#include "gfx/context.h"
#include "gfx/renderer.h"

static void on_window_close(event_bus_t *bus, void *user_pointer, window_close_event_t *event)
{
	app_state_t *state = (app_state_t *)user_pointer;
	state->running = false;
}

static void init_libs()
{
	HE_VERIFY(glfwInit(), "Failed to initialize GLFW");
	stbi_set_flip_vertically_on_load(true);
}

static void shutdown_libs()
{
	glfwTerminate();
}

static void init_resources(app_state_t *state)
{
	camera_init(&(camera_desc_t){
		.fov = 70.0f,
		.angle = { 0.0f, 45.0f, },
		.distance = -50.0f,
		.window = state->window,
	}, &state->camera);

	terrain_init(&(terrain_desc_t){
		.position = { 0.0f, 0.0f, 0.0f },
		.size = { 50, 50 },
	}, &state->terrain);
}

static void free_resources(app_state_t *state)
{
	terrain_free(state->terrain);
}

bool app_init(app_state_t *state)
{
	memset(state, 0, sizeof(state));
	state->running = true;

	init_libs();

	// prepare the environment
	state->event_bus = event_bus_create(&(event_bus_desc_t){ 0 });

	state->window = window_create(&(window_desc_t){
		.title = APP_NAME,
		.size = { 1280, 720 },
		.resizable = true,
		.samples = 16,
		.event_bus = state->event_bus,
	});

	context_bind(state->window->context);

	// initialize the resources (duh...)
	init_resources(state);

	// subscribe to events
	event_subscribe(state->event_bus, EVENT_TYPE_WINDOW_CLOSE, state, (event_callback_fn_t)on_window_close);

	return true;
}

void app_run(app_state_t *state)
{
	while (state->running && window_process_events(state->window))
	{
		renderer_clear(&(cmd_clear_desc_t){
			.color = { 1.0f, 1.0f, 1.0f, 1.0f },
			.depth = 1,
		});

		camera_set_target(state->camera, state->terrain->position);
		terrain_draw(state->camera, state->terrain);

		window_swap_buffers(state->window);
	}
}

void app_shutdown(app_state_t *state)
{
	free_resources(state);
	window_free(state->window);
	shutdown_libs();
}
