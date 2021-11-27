#include "app.h"

#include <string.h>
#include <time.h>
#include <math.h>
#include <limits.h>

#include <stb_image.h>

#include "debug/assert.h"
#include "events/window_event.h"
#include "gfx/context.h"
#include "gfx/renderer.h"
#include "math/noise.h"
#include "erosion.h"

static bool on_window_close(event_bus_t *bus, bool handled, void *user_pointer, window_close_event_t *event)
{
	app_state_t *state = (app_state_t *)user_pointer;
	state->running = false;
	return true;
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
	state->config = APP_DEFAULT_CONFIGURATION;
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

static void on_app_configure(app_state_t *state, float delta)
{
	if (igBegin("Configuration", NULL, ImGuiWindowFlags_None))
	{
		// animation settings
		if (igTreeNodeEx_Str("Animation", ImGuiTreeNodeFlags_DefaultOpen))
		{
			igCheckbox("Animate", &state->config.animate);
			if (!state->config.animate) 
			{
				igPushItemFlag(ImGuiItemFlags_Disabled, true);
				igPushStyleVar_Float(ImGuiStyleVar_Alpha, igGetStyle()->Alpha * 0.5f);
			}

			igSliderInt("Duration", &state->config.duration, 1, 60, "%d sec", 0);

			if (!state->config.animate)
			{
				igPopItemFlag();
				igPopStyleVar(1);
			}

			igTreePop();
		}

		// terrain settings
		if (igTreeNodeEx_Str("Terrain", ImGuiTreeNodeFlags_DefaultOpen))
		{
			bool reset = false;
			reset |= igInputInt("Seed", &state->terrain->seed, INT_MIN, INT_MAX, ImGuiInputTextFlags_EnterReturnsTrue);
			reset |= igDragInt2("Size", state->terrain->size.size, 1, 2, 10000, "%d", 0);
			reset |= igDragFloat("Scale", &state->terrain->scale_scalar, 0.05f, 0.01f, 100.0f, "%.2f", 0);

			if (reset)
			{
				terrain_reset(state->terrain);
			}
			igTreePop();
		}

		// simulation settings
		if (igTreeNodeEx_Str("Simulation", ImGuiTreeNodeFlags_DefaultOpen))
		{
			igInputInt("Iterations", &state->config.iterations, 1, 100, 0);
			igTreePop();
		}

		// start the simulation
		if (igButton("Start", (ImVec2){ 0, 0 }))
		{
			memset(&state->sim_data, 0, sizeof(state->sim_data));
			state->mode = APP_MODE_SIMULATE;
		}
	}
	igEnd();
}

static void on_app_simulate(app_state_t *state, float delta)
{
	if (!state->config.animate)
	{
		float start = glfwGetTime();
		run_simulation(state->terrain, state->config.iterations);
		float end = glfwGetTime();

		state->sim_data.cur_iterations = state->config.iterations;
		state->sim_data.duration = end - start;
		state->mode = APP_MODE_COMPLETE;
	}
	else
	{
		float delta_seconds = delta / 1000.0f;

		int delta_iter = (int)((float)state->config.iterations / (float)state->config.duration) * delta_seconds;
		int remaining_iter = state->config.iterations - state->sim_data.cur_iterations;
		int iterations = fmin((float)remaining_iter, delta_iter);
		run_simulation(state->terrain, iterations);

		state->sim_data.cur_iterations += iterations;
		state->sim_data.duration += delta_seconds;

		if (igBegin("Simulation Data", NULL, 0))
		{
			float progress = ((float)state->sim_data.cur_iterations / (float)state->config.iterations);
			igProgressBar(progress, (ImVec2){ -FLT_MIN, 0 }, "Progress");
			igValue_Int("Iterations", state->sim_data.cur_iterations);
			igText("Duration: %f", state->sim_data.duration);
		}
		igEnd();

		if (state->sim_data.cur_iterations >= state->config.iterations)
		{
			state->mode = APP_MODE_COMPLETE;
		}
	}
}

static void on_app_complete(app_state_t *state, float delta)
{
	ImGuiIO *io = igGetIO();
	igSetNextWindowPos((ImVec2){ io->DisplaySize.x * 0.5f, io->DisplaySize.y * 0.5f }, ImGuiCond_Once, (ImVec2){ 0.5f,0.5f });
	if (igBegin("Simulation Complete", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings))
	{
		igText("Simulation complete!");
		igText("%d iterations run in %f seconds", state->sim_data.cur_iterations, state->sim_data.duration);
		bool reset = igButton("Reset", (ImVec2){ 0, 0 }); igSameLine(0.0f, -1.0f);
		bool continue_ = igButton("Continue", (ImVec2){ 0, 0 });

		if (reset)
		{
			state->mode = APP_MODE_CONFIGURE;
			terrain_reset(state->terrain);
		}
		else if (continue_)
		{
			state->mode = APP_MODE_CONFIGURE;
		}
	}
	igEnd();
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
	event_subscribe(state->event_bus, EVENT_TYPE_WINDOW_CLOSE, EVENT_LAYER_APP, state, (event_callback_fn_t)on_window_close);

	return true;
}

void app_run(app_state_t *state)
{
	float last_time = glfwGetTime() * 1000.0f;
	while (state->running && window_process_events(state->window))
	{
		// calculate delta time
		float current_time = glfwGetTime() * 1000.0f;
		float delta = current_time - last_time;
		last_time = current_time;

		// start drawing to imgui
		imgui_context_begin(state->imgui);

		// run app logic
		switch (state->mode)
		{
		case APP_MODE_CONFIGURE:
			on_app_configure(state, delta);
			break;
		case APP_MODE_SIMULATE:
			on_app_simulate(state, delta);
			break;
		case APP_MODE_COMPLETE:
			on_app_complete(state, delta);
			break;
		}

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
