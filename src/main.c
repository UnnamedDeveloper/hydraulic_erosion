#include <stdint.h>
#include <stdio.h>

#include <cglm/cglm.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "debug/assert.h"
#include "gfx/buffer.h"
#include "gfx/image.h"
#include "gfx/pipeline.h"
#include "gfx/renderer.h"
#include "gfx/window.h"

#define ARRAY_LENGTH(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

typedef struct vertex_t
{
	vec3 position;
	vec2 uv;
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
		{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f } },
		{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f } },
	};

	uint32_t indices[] = {
		0, 2, 1,
		2, 1, 3,
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

	// create image
	stbi_set_flip_vertically_on_load(true);

	uvec2 image_size;
	int image_channels;
	uint8_t *image_data = stbi_load("res/images/dope_frog.png", &image_size.w, &image_size.h, &image_channels, STBI_rgb_alpha);

	image_t *image1 = image_create(&(image_desc_t){
		.type = IMAGE_TYPE_2D,
		.size = image_size,
		.format = IMAGE_FORMAT_RGBA8,
		.data = image_data,
		.wrap_s = IMAGE_WRAP_REPEAT,
		.wrap_t = IMAGE_WRAP_REPEAT,
		.min = IMAGE_FILTER_LINEAR,
		.mag = IMAGE_FILTER_LINEAR
	});

	stbi_image_free(image_data);

	image_data = stbi_load("res/images/dope_guy.png", &image_size.w, &image_size.h, &image_channels, STBI_rgb_alpha);

	image_t *image2 = image_create(&(image_desc_t){
		.type = IMAGE_TYPE_2D,
		.size = image_size,
		.format = IMAGE_FORMAT_RGBA8,
		.data = image_data,
		.wrap_s = IMAGE_WRAP_REPEAT,
		.wrap_t = IMAGE_WRAP_REPEAT,
		.min = IMAGE_FILTER_LINEAR,
		.mag = IMAGE_FILTER_LINEAR
	});

	stbi_image_free(image_data);

	// create shader pipeline
	const char *vs_source =
		"#version 330 core\n"
		"layout(location = 0) in vec3 i_pos;\n"
		"layout(location = 1) in vec2 i_uv;\n"
		"out vec2 v_uv;\n"
		"uniform mat4 u_model;\n"
		"uniform mat4 u_view;\n"
		"uniform mat4 u_projection;\n"
		"void main() {\n"
		"	gl_Position = u_projection * u_view * u_model * vec4(i_pos, 1.0);\n"
		"	v_uv = i_uv;\n"
		"}\n";

	const char *fs_source =
		"#version 330 core\n"
		"in vec2 v_uv;\n"
		"out vec4 o_col;\n"
		"uniform float u_time;\n"
		"uniform sampler2D tex;\n"
		"uniform sampler2D tex2;\n"
		"void main() {\n"
		"	o_col = mix(texture(tex, v_uv), texture(tex2, v_uv), sin(u_time));\n"
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
			.location[1] = { .type = ATTRIBUTE_TYPE_FLOAT2, .offset = offsetof(vertex_t, uv) },
			.stride = sizeof(vertex_t),
		},
		.uniforms = {
			.location[0] = { .type = UNIFORM_TYPE_MAT4, .name = "u_model" },
			.location[1] = { .type = UNIFORM_TYPE_MAT4, .name = "u_view" },
			.location[2] = { .type = UNIFORM_TYPE_MAT4, .name = "u_projection" },
			.location[3] = { .type = UNIFORM_TYPE_FLOAT, .name = "u_time" },
		},
		.images = {
			.location[0] = { .name = "tex", .type = IMAGE_TYPE_2D },
			.location[1] = { .name = "tex2", .type = IMAGE_TYPE_2D },
		},
	});

	// main loop
	while (window_process_events(window))
	{
		uvec2 size = window_get_size(window);
		renderer_set_viewport(0, 0, size.w, size.h);

		renderer_clear(&(cmd_clear_desc_t){ .color = { 1.0f, 1.0f, 1.0f, 1.0f }, });

		buffer_bind(vertex_data);
		buffer_bind(index_data);
		image_bind(0, image1);
		image_bind(1, image2);
		pipeline_bind(pipeline);

		float time = glfwGetTime();

		mat4 model = GLM_MAT4_IDENTITY_INIT;
		glm_rotate(model, time, (vec3){ 0.0f, 0.0f, 1.0f });

		mat4 view = GLM_MAT4_IDENTITY_INIT;
		glm_translate(view, (vec3){ 0.0f, 0.0f, -3.0f });

		mat4 projection = GLM_MAT4_IDENTITY_INIT;
		glm_perspective(glm_rad(70.0f), (float)size.w / (float)size.h, 0.1f, 1000.0f, projection);

		pipeline_set_uniform_mat4(pipeline, 0, model);
		pipeline_set_uniform_mat4(pipeline, 1, view);
		pipeline_set_uniform_mat4(pipeline, 2, projection);
		pipeline_set_uniformf(pipeline, 3, time);

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
