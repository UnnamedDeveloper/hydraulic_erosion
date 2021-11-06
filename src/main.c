#include <stdint.h>
#include <stdio.h>

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "debug/assert.h"
#include "gfx/buffer.h"
#include "gfx/window.h"

#define ARRAY_LENGTH(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

typedef struct vertex_t
{
	vec3 position;
} vertex_t;

int main()
{
	HE_VERIFY(glfwInit());

	// create window & context
	window_t *window = window_create(&(window_desc_t){
		.title = "Hydraulic Erosion",
		.size = { 1280, 720 },
	});

	context_bind(window->context);

	// create buffer
	vertex_t vertices[] = {
		{ {  0.0f,  0.5f, 0.0f, } },
		{ { -0.5f, -0.5f, 0.0f, } },
		{ {  0.5f, -0.5f, 0.0f, } },
	};

	uint32_t indices[] = {
		0, 1, 2
	};

	buffer_t *vertex_data = buffer_create(&(buffer_desc_t){
		.size = sizeof(vertices),
		.data = vertices,
		.type = BUFFER_TYPE_VERTEX,
		.usage = BUFFER_USAGE_STATIC,
	});

	buffer_t *index_data = buffer_create(&(buffer_desc_t){
		.size = sizeof(indices),
		.data = indices,
		.type = BUFFER_TYPE_INDEX,
		.usage = BUFFER_USAGE_STATIC,
	});

	buffer_bind(vertex_data);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), 0);
	glEnableVertexAttribArray(0);

	// main loop
	glClearColor(1, 0, 1, 1);
	while (window_process_events(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		buffer_bind(vertex_data);
		buffer_bind(index_data);
		glDrawElements(GL_TRIANGLES, ARRAY_LENGTH(indices), GL_UNSIGNED_INT, 0);
		window_swap_buffers(window);
	}

	// free resources
	buffer_free(vertex_data);
	buffer_free(index_data);

	window_free(window);

	glfwTerminate();
	return 0;
}
