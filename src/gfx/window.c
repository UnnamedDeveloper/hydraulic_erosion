#include "window.h"

#include <stdlib.h>

#include "debug/assert.h"

void window_init(const window_desc_t *desc, window_t **window)
{
	HE_ASSERT(window != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A window description is required");
	HE_ASSERT(desc->w > 0, "Window width must be larger than 0");
	HE_ASSERT(desc->h > 0, "Window height must be larger than 0");
	HE_ASSERT(desc->title != NULL, "A window title is required");

	window_t *result = calloc(1, sizeof(window_t));

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, desc->resizable);
	glfwWindowHint(GLFW_SAMPLES, desc->samples);
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	result->glfw_window = glfwCreateWindow(desc->w, desc->h, desc->title, NULL, NULL);

	context_init(&(context_desc_t){
		.window = result,
	#ifndef NDEBUG
		.debug = true,
	#endif
	}, &result->context);

	*window = result;
}

window_t *window_create(const window_desc_t *desc)
{
	window_t *window;
	window_init(desc, &window);
	return window;
}

void window_free(window_t *window)
{
	context_free(window->context);
	glfwDestroyWindow(window->glfw_window);
	free(window);
}

bool window_process_events(window_t *window)
{
	glfwPollEvents();
	return !glfwWindowShouldClose(window->glfw_window);
}

void window_swap_buffers(window_t *window)
{
	glfwSwapBuffers(window->glfw_window);
}
