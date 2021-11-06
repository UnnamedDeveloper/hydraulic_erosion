#include "window.h"

#include "debug/assert.h"

void window_init(const window_desc_t *desc, window_t **window)
{
	HE_ASSERT(desc != NULL);
	HE_ASSERT(window != NULL);

	*window = glfwCreateWindow(desc->w, desc->h, desc->title, NULL, NULL);
}

window_t *window_create(const window_desc_t *desc)
{
	window_t *window;
	window_init(desc, &window);
	return window;
}

void window_free(window_t *window)
{
	glfwDestroyWindow(window);
}

bool window_process_events(window_t *window)
{
	glfwPollEvents();
	return !glfwWindowShouldClose(window);
}

void window_swap_buffers(window_t *window)
{
	glfwSwapBuffers(window);
}
