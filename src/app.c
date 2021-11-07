#include "app.h"

#include <string.h>

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "debug/assert.h"
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

bool app_init(app_state_t *state)
{
	memset(state, 0, sizeof(state));

	init_libs();

	state->window = window_create(&(window_desc_t){
		.title = APP_NAME,
		.size = { 1280, 720 },
		.resizable = true,
		.samples = 16,
	});

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
