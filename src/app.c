#include "app.h"

#include <string.h>
#include <time.h>
#include <math.h>

#include <stb_image.h>

#include "debug/assert.h"
#include "events/window_event.h"
#include "gfx/context.h"
#include "gfx/renderer.h"
#include "math/noise.h"
#include "erosion.h"

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
		.sensitivity = 0.25f,
		.angle = { 0.0f, 45.0f, },
		.distance = -150.0f,
		.window = state->window,
	}, &state->camera);

	terrain_init(&(terrain_desc_t){
		.position = { 0.0f, 0.0f, 0.0f },
		.size = { 500, 500 },
		.noise_function = (terrain_noise_function_t)perlin_noise_2d,
		.seed = time(0),
		.scale_scalar = 0.4f,
		.elevation = 100.0f,
	}, &state->terrain);
}

static void free_resources(app_state_t *state)
{
	terrain_free(state->terrain);
	camera_free(state->camera);
}

static void run_simulation(terrain_t *t, int iterations)
{
	for (int i = 0; i < iterations; i++)
	{
		hydraulic_erosion(t);
	}
	terrain_update_mesh(t);
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

	// setup imgui
	state->imgui = imgui_context_create(&(imgui_context_desc_t) {
		.window = state->window,
		.event_bus = state->event_bus,
	});

	// initialize the resources (duh...)
	init_resources(state);

	// subscribe to events
	event_subscribe(state->event_bus, EVENT_TYPE_WINDOW_CLOSE, state, (event_callback_fn_t)on_window_close);

	return true;
}

void app_run(app_state_t *state)
{
	bool animate = true;

	int total_iterations = 200000;
	int frame_step_count = 500;

	if (!animate)
	{
		run_simulation(state->terrain, total_iterations);
	}

	int run_iterations = 0;
	float last_time = glfwGetTime() * 1000.0f;
	while (state->running && window_process_events(state->window))
	{
		// calculate delta time
		float current_time = glfwGetTime() * 1000.0f;
		float delta = current_time - last_time;
		last_time = current_time;
		
		// update
		if (animate && total_iterations > run_iterations)
		{
			int steps = fmin((total_iterations - run_iterations), frame_step_count);
			run_simulation(state->terrain, steps);
			run_iterations += steps;
		}

		// draw ui
		imgui_context_begin(state->imgui, delta);

		igShowDemoWindow(NULL);

		imgui_context_end(state->imgui);
		
		// render
		renderer_clear(&(cmd_clear_desc_t){
			.color = { 1.0f, 1.0f, 1.0f, 1.0f },
			.depth = 1,
		});

		terrain_draw(state->camera, (vec3) { 0.0f, 100.0f, 0.0f }, state->terrain);

		imgui_context_render(state->imgui);

		window_swap_buffers(state->window);
	}
}

void app_shutdown(app_state_t *state)
{
	free_resources(state);
	imgui_context_free(state->imgui);
	window_free(state->window);
	shutdown_libs();
}
