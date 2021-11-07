#include <stdint.h>
#include <stdio.h>

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "debug/assert.h"
#include "gfx/buffer.h"
#include "gfx/pipeline.h"
#include "gfx/renderer.h"
#include "gfx/window.h"

#define ARRAY_LENGTH(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

typedef struct vertex_t
{
	vec3 position;
	vec3 color;
} vertex_t;

int main()
{
	HE_VERIFY(glfwInit(), "Failed to initialize GLFW");

	// create window & context
	window_t *window = window_create(&(window_desc_t){
		.title = "Hydraulic Erosion",
		.size = { 1280, 720 },
		.samples = 16,
		.resizable = true,
	});

	context_bind(window->context);

	// create buffer
	vertex_t vertices[] = {
		{ {  0.0f,  0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
	};

	uint32_t indices[] = {
		0, 1, 2,
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

	// create shader pipeline
	const char *vs_source =
		"#version 330 core\n"
		"layout(location = 0) in vec3 i_pos;\n"
		"layout(location = 1) in vec3 i_col;\n"
		"out vec3 v_col;\n"
		"uniform mat4 u_model;\n"
		"uniform mat4 u_view;\n"
		"uniform mat4 u_projection;\n"
		"void main() {\n"
		"	gl_Position = u_projection * u_view * u_model * vec4(i_pos, 1.0);\n"
		"	v_col = i_col;\n"
		"}\n";

	const char *fs_source =
		"#version 330 core\n"
		"in vec3 v_col;\n"
		"layout(location = 0) out vec4 o_col;\n"
		"void main() {\n"
		"	o_col = vec4(v_col, 1.0);\n"
		"}\n";

	shader_t *vs_shader = shader_create(&(shader_desc_t){
		.source = vs_source,
		.type = SHADER_TYPE_VERTEX,
	});

	shader_t *fs_shader = shader_create(&(shader_desc_t){
		.source = fs_source,
		.type = SHADER_TYPE_FRAGMENT,
	});
	
	pipeline_t *pipeline = pipeline_create(&(pipeline_desc_t){
		.vs = vs_shader,
		.fs = fs_shader,
		.layout = {
			.location[0] = { .type = ATTRIBUTE_TYPE_FLOAT3, .offset = offsetof(vertex_t, position) },
			.location[1] = { .type = ATTRIBUTE_TYPE_FLOAT3, .offset = offsetof(vertex_t, color) },
			.stride = sizeof(vertex_t),
		},
		.uniforms = {
			.location[0] = { .type = UNIFORM_TYPE_MAT4, .name = "u_model" },
			.location[1] = { .type = UNIFORM_TYPE_MAT4, .name = "u_view" },
			.location[2] = { .type = UNIFORM_TYPE_MAT4, .name = "u_projection" },
		}
	});

	// main loop
	while (window_process_events(window))
	{
		uvec2 size = window_get_size(window);
		renderer_set_viewport(0, 0, size.w, size.h);

		renderer_clear(&(cmd_clear_desc_t){ .color = { 1.0f, 1.0f, 1.0f, 1.0f }, });

		buffer_bind(vertex_data);
		buffer_bind(index_data);
		pipeline_bind(pipeline);

		mat4 model = GLM_MAT4_IDENTITY_INIT;
		glm_rotate(model, glfwGetTime(), (vec3){ 0.0f, 0.0f, 1.0f });

		mat4 view = GLM_MAT4_IDENTITY_INIT;
		glm_translate(view, (vec3){ 0.0f, 0.0f, -3.0f });

		mat4 projection = GLM_MAT4_IDENTITY_INIT;
		glm_perspective(glm_rad(70.0f), (float)size.w / (float)size.h, 0.1f, 1000.0f, projection);

		pipeline_set_uniform(pipeline, 0, model);
		pipeline_set_uniform(pipeline, 1, view);
		pipeline_set_uniform(pipeline, 2, projection);

		renderer_draw_indexed(ARRAY_LENGTH(indices));

		window_swap_buffers(window);
	}

	// free resources
	shader_free(vs_shader);
	shader_free(fs_shader);
	pipeline_free(pipeline);
	buffer_free(vertex_data);
	buffer_free(index_data);

	window_free(window);

	glfwTerminate();
	return 0;
}
