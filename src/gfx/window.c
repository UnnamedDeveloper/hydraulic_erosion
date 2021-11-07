#include "window.h"

#include <stdlib.h>

#include "events/window_event.h"
#include "debug/assert.h"

void on_window_close(GLFWwindow *glfw_window)
{
	window_t *window = (window_t *)glfwGetWindowUserPointer(glfw_window);
	if (window->bus != NULL)
	{
		window_close_event_t event = { .window = window };
		event_publish(window->bus, EVENT_TYPE_WINDOW_CLOSE, (event_t *)&event);
	}
}

void on_window_resize(GLFWwindow *glfw_window, int width, int height)
{
	window_t *window = (window_t *)glfwGetWindowUserPointer(glfw_window);
	if (window->bus != NULL)
	{
		window_resize_event_t event = { .window = window, .size = { (uint32_t)width, (uint32_t)height } };
		event_publish(window->bus, EVENT_TYPE_WINDOW_RESIZE, (event_t *)&event);
	}
}

void window_init(const window_desc_t *desc, window_t **window)
{
	HE_ASSERT(window != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A window description is required");
	HE_ASSERT(desc->size.w > 0, "Window width must be larger than 0");
	HE_ASSERT(desc->size.h > 0, "Window height must be larger than 0");
	HE_ASSERT(desc->title != NULL, "A window title is required");

	window_t *result = calloc(1, sizeof(window_t));

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, desc->resizable);
	glfwWindowHint(GLFW_SAMPLES, desc->samples);
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

	result->glfw_window = glfwCreateWindow(desc->size.w, desc->size.h, desc->title, NULL, NULL);
	HE_ASSERT(result->glfw_window, "Failed to create GLFW window");

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
	if (window == NULL) return;
	
	context_free(window->context);
	glfwDestroyWindow(window->glfw_window);
	free(window);
}

void window_attach_event_bus(window_t *window, event_bus_t *bus)
{
	window->bus = bus;
	glfwSetWindowUserPointer(window->glfw_window, window);

	glfwSetWindowCloseCallback(window->glfw_window, on_window_close);
	glfwSetWindowSizeCallback(window->glfw_window, on_window_resize);
}

uvec2 window_get_size(window_t *window)
{
	uvec2 size;
	glfwGetFramebufferSize(window->glfw_window, &size.w, &size.h);
	return size;
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
