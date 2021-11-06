#ifndef __gfx_window_h__
#define __gfx_window_h__

#include <stdbool.h>

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

#include "context.h"

typedef struct window_desc_t
{
	const char *title;
	union
	{
		uint32_t size[2];
		struct { uint32_t width, height; };
		struct { uint32_t w, h; };
	};
	bool resizable;
} window_desc_t;

typedef struct window_t
{
	GLFWwindow *glfw_window;
	context_t *context;
} window_t;

void window_init(const window_desc_t *desc, window_t **window);
window_t *window_create(const window_desc_t *desc);
void window_free(window_t *window);

bool window_process_events(window_t *window);
void window_swap_buffers(window_t *window);

#endif /* __gfx_window_h__ */
