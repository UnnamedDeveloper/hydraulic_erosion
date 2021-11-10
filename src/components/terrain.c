#include "terrain.h"

#include <stdlib.h>

#include "debug/assert.h"
#include "io/file.h"

typedef struct terrain_vertex_t
{
	vec3 position;
	vec3 color;
} terrain_vertex_t;

static shader_t *create_shader(const char *path, shader_type_t type)
{
	size_t file_size = 0;
	FILE *file = NULL;

	file = file_open(path);

	HE_ASSERT(file != NULL, "Failed to open shader file");

	file_read(file, &file_size, NULL);
	char *source = calloc(1, file_size + 1);
	file_read(file, &file_size, source);
	source[file_size] = '\0';
	file_close(file);

	shader_t *shader = shader_create(&(shader_desc_t){
		.type = type,
		.source = source,
	});

	free(source);

	return shader;
}

static void terrain_init_pipeline(terrain_t *terrain)
{
	shader_t *vs = create_shader("res/shaders/terrain.vs.glsl", SHADER_TYPE_VERTEX);
	shader_t *fs = create_shader("res/shaders/terrain.fs.glsl", SHADER_TYPE_FRAGMENT);

	pipeline_desc_t desc = {
		.vs = vs,
		.fs = fs,
		.layout = {
			.location[0] = {.type = ATTRIBUTE_TYPE_FLOAT3, .offset = offsetof(terrain_vertex_t, position), },
			.location[1] = {.type = ATTRIBUTE_TYPE_FLOAT3, .offset = offsetof(terrain_vertex_t, color),    },
			.stride = sizeof(terrain_vertex_t),
		},
		.uniforms = {
			.location[0] = {.type = UNIFORM_TYPE_MAT4, .name = "u_model",      },
			.location[1] = {.type = UNIFORM_TYPE_MAT4, .name = "u_view",       },
			.location[2] = {.type = UNIFORM_TYPE_MAT4, .name = "u_projection", },
		},
		.depth_test = true,
		.culling = true,
	};

	pipeline_init(&desc, &terrain->pipeline);
#ifndef NDEBUG
	shader_t *wireframe_fs = create_shader("res/shaders/terrain_wireframe.fs.glsl", SHADER_TYPE_FRAGMENT);

	desc.primitive_type = PRIMITIVE_TYPE_LINE_STRIP;
	desc.fs = wireframe_fs;
	desc.depth_test = false;
	pipeline_init(&desc, &terrain->pipeline_wireframe);

	shader_free(wireframe_fs);
#endif

	shader_free(vs);
	shader_free(fs);
}

static void terrain_init_mesh(terrain_t *terrain)
{
	mesh_init(&(mesh_desc_t){
		.dynamic = true,
	}, &terrain->mesh);
}

void terrain_init(const terrain_desc_t *desc, terrain_t **terrain)
{
	HE_ASSERT(terrain != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A terrain description is required");
	HE_ASSERT(desc->noise_function != NULL, "A terrain noise function is required");

	terrain_t *result = calloc(1, sizeof(terrain_t));

	result->noise_function = desc->noise_function;

	terrain_init_pipeline(result);
	terrain_init_mesh(result);

	terrain_resize(result, desc->size);

	glm_vec3_copy(desc->position, result->position);

	*terrain = result;
}

terrain_t *terrain_create(const terrain_desc_t *desc)
{
	terrain_t *terrain;
	terrain_init(desc, &terrain);
	return terrain;
}

void terrain_free(terrain_t *terrain)
{
	if (terrain == NULL) return;

	mesh_free(terrain->mesh);
	pipeline_free(terrain->pipeline);
#ifndef NDEBUG
	pipeline_free(terrain->pipeline_wireframe);
#endif
	free(terrain);
}

void terrain_draw(camera_t *camera, terrain_t *terrain)
{
	HE_ASSERT(terrain != NULL, "Cannot draw NULL terrain");
	HE_ASSERT(camera != NULL, "Cannot draw terrain with NULL camera");

	// calculate the cordinate system
	mat4 model = GLM_MAT4_IDENTITY_INIT;
	glm_translate(model, terrain->position);

	mat4 view  = GLM_MAT4_IDENTITY_INIT;
	camera_create_view_matrix(camera, view);

	// draw the terrain
	pipeline_t *last_pip = pipeline_bind(terrain->pipeline);

	pipeline_set_uniform_mat4(terrain->pipeline, 0, model);
	pipeline_set_uniform_mat4(terrain->pipeline, 1, view);
	pipeline_set_uniform_mat4(terrain->pipeline, 2, camera->projection);

	mesh_draw(terrain->mesh);

	// draw wireframe
#ifndef NDEBUG
	pipeline_bind(terrain->pipeline_wireframe);

	pipeline_set_uniform_mat4(terrain->pipeline_wireframe, 0, model);
	pipeline_set_uniform_mat4(terrain->pipeline_wireframe, 1, view);
	pipeline_set_uniform_mat4(terrain->pipeline_wireframe, 2, camera->projection);

	mesh_draw(terrain->mesh);
#endif

	// restore previous state
	pipeline_bind(last_pip);
}

void terrain_resize(terrain_t *terrain, uvec2 size)
{
	HE_ASSERT(terrain != NULL, "Cannot resize NULL");
	HE_ASSERT(size.w > 0 || size.h > 0, "Invalid terrain size");

	size_t vertex_count = size.w * size.h;
	terrain_vertex_t *vertices = calloc(vertex_count, sizeof(terrain_vertex_t));

	size_t index_count = (size.w - 1) * (size.h - 1) * 6;
	int *indices = calloc(index_count, sizeof(int));

	// thanks brackeys
	int i = 0;
	for (int z = 0; z < size.h; z++)
	{
		for (int x = 0; x < size.w; x++)
		{
			vertices[i].position[0] = (float)x - (size.w / 2.0f);
			vertices[i].position[1] = terrain->noise_function(x, z);
			vertices[i].position[2] = (float)z - (size.h / 2.0f);

			vertices[i].color[0] = ((float)x + 1.0f) / size.w;
			vertices[i].color[1] = ((float)x + 1) * ((float)z + 1) / (size.w * size.h);
			vertices[i].color[2] = ((float)z + 1.0f) / size.h;

			i++;
		}
	}

	int vertex = 0;
	int index = 0;
	for (int z = 0; z < size.h - 1; z++)
	{
		for (int x = 0; x < size.w - 1; x++)
		{
			indices[index + 0] = vertex;
			indices[index + 1] = vertex + (size.w - 1) + 1;
			indices[index + 2] = vertex + 1;
			indices[index + 3] = vertex + 1;
			indices[index + 4] = vertex + (size.w - 1) + 1;
			indices[index + 5] = vertex + (size.w - 1) + 2;

			vertex++;
			index += 6;
		}
		vertex++;
	}

	mesh_set_data(terrain->mesh, &(mesh_desc_t){
		.vertices = vertices,
		.vertices_size = vertex_count * sizeof(terrain_vertex_t),
		.indices = indices,
		.indices_size = index_count * sizeof(int),
		.index_count = index_count,
	});

	free(vertices);
	free(indices);
}
