#include "app.h"

#include <string.h>

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "debug/assert.h"
#include "events/window_event.h"
#include "gfx/context.h"
#include "gfx/renderer.h"

static void init_libs()
{
	HE_VERIFY(glfwInit(), "Failed to initialize GLFW");
	stbi_set_flip_vertically_on_load(true);
}

static void shutdown_libs()
{
	glfwTerminate();
}

static void on_window_close(event_bus_t *bus, window_close_event_t *event)
{
	app_state_t *state = (app_state_t *)bus->user_pointer;

}

static void on_window_resize(event_bus_t *bus, window_resize_event_t *event)
{
	context_t *last_ctx = context_bind(event->window->context);
	renderer_set_viewport(0, 0, event->size.w, event->size.h);
	printf("the window was resized: %ix%i\n", (int)event->size.w, (int)event->size.h);
	context_bind(last_ctx);
}

bool app_init(app_state_t *state)
{
	memset(state, 0, sizeof(state));

	init_libs();

	state->event_bus = event_bus_create(&(event_bus_desc_t){
		.user_pointer = state,
	});

	event_subscribe(state->event_bus, EVENT_TYPE_WINDOW_CLOSE, (event_callback_fn_t)on_window_close);
	event_subscribe(state->event_bus, EVENT_TYPE_WINDOW_RESIZE, (event_callback_fn_t)on_window_resize);

	state->window = window_create(&(window_desc_t){
		.title = APP_NAME,
		.size = { 1280, 720 },
		.resizable = true,
		.samples = 16,
	});
	window_attach_event_bus(state->window, state->event_bus);

	context_bind(state->window->context);

	return true;
}

void app_run(app_state_t *state)
{
	while (window_process_events(state->window))
	{
		uvec2 size = window_get_size(state->window);
		renderer_set_viewport(0, 0, size.w, size.h);

		renderer_clear(&(cmd_clear_desc_t){
			.color = { 1.0f, 1.0f, 1.0f, 1.0f },
		});

		window_swap_buffers(state->window);
	}
}

void app_shutdown(app_state_t *state)
{
	window_free(state->window);
	shutdown_libs();
}
