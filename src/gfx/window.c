#include "window.h"

#include <stdlib.h>

#include "debug/assert.h"

void window_init(const window_desc_t *desc, window_t **window)
{
	HE_ASSERT(desc != NULL);
	HE_ASSERT(window != NULL);

	window_t *result = calloc(1, sizeof(window_t));

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, desc->resizable);

	result->glfw_window = glfwCreateWindow(desc->w, desc->h, desc->title, NULL, NULL);

	context_init(&(context_desc_t){
		.window = result,
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
