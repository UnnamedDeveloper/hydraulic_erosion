#include <stdio.h>

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

	while (window_process_events(window))
	{
		window_swap_buffers(window);
	}

	window_free(window);
	return 0;
}
