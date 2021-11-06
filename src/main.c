#include <stdio.h>

#include <glad/glad.h>

#include "debug/assert.h"
#include "gfx/window.h"

int main()
{
	HE_VERIFY(glfwInit());

	window_t *window = window_create(&(window_desc_t){
		.title = "Hydraulic Erosion",
		.width = 1280,
		.height = 720,
	});

	context_bind(window->context);

	glClearColor(1, 1, 1, 1);
	while (window_process_events(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		window_swap_buffers(window);
	}

	window_free(window);
	return 0;
}
