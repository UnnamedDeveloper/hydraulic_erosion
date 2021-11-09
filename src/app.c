#include "app.h"

#include <stdio.h>
#include <string.h>

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "debug/assert.h"
#include "events/mouse_event.h"
#include "events/window_event.h"
#include "gfx/context.h"
#include "gfx/renderer.h"
#include "io/file.h"

static void on_window_close(event_bus_t *bus, void *user_pointer, window_close_event_t *event)
{
	app_state_t *state = (app_state_t *)user_pointer;
	state->running = false;
}

static void on_window_resize(event_bus_t *bus, void *user_pointer, window_resize_event_t *event)
{
	context_t *last_ctx = context_bind(event->window->context);
	renderer_set_viewport(0, 0, event->size.w, event->size.h);

	app_state_t *state = (app_state_t *)user_pointer;
	camera_update_projection(&state->camera, window_get_size(event->window));

	context_bind(last_ctx);
}

static void on_mouse_move(event_bus_t *bus, void *user_pointer, mouse_move_event_t *event)
{
	app_state_t *state = (app_state_t *)user_pointer;
	camera_move(&state->camera, 0, (vec2){ -event->offset[0] / 10.0f, event->offset[1] / 10.0f });
}

static void on_mouse_scroll(event_bus_t *bus, void *user_pointer, mouse_scroll_event_t *event)
{
	app_state_t *state = (app_state_t *)user_pointer;
	camera_move(&state->camera, event->offset[1], GLM_VEC2_ZERO);
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
	camera_init(70.0f, -5.0f, window_get_size(state->window), &state->camera);

	terrain_init(&(terrain_desc_t){
		.position = { 0.0f, 0.0f, 0.0f },
		.size = { 10, 10 },
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
	state->event_bus = event_bus_create(&(event_bus_desc_t){
	});

	state->window = window_create(&(window_desc_t){
		.title = APP_NAME,
		.size = { 1280, 720 },
		.resizable = true,
		.samples = 16,
	});
	window_attach_event_bus(state->window, state->event_bus);

	context_bind(state->window->context);

	// initialize the resources (duh...)
	init_resources(state);

	// subscribe to events
	event_subscribe(state->event_bus, EVENT_TYPE_WINDOW_CLOSE, state, (event_callback_fn_t)on_window_close);
	event_subscribe(state->event_bus, EVENT_TYPE_WINDOW_RESIZE, state, (event_callback_fn_t)on_window_resize);
	event_subscribe(state->event_bus, EVENT_TYPE_MOUSE_MOVE, state, (event_callback_fn_t)on_mouse_move);
	event_subscribe(state->event_bus, EVENT_TYPE_MOUSE_SCROLL, state, (event_callback_fn_t)on_mouse_scroll);

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

		camera_set_target(&state->camera, state->terrain->position);
		terrain_draw(&state->camera, state->terrain);

		window_swap_buffers(state->window);
	}
}

void app_shutdown(app_state_t *state)
{
	free_resources(state);
	window_free(state->window);
	shutdown_libs();
}
