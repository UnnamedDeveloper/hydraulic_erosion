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

	pipeline_init(&(pipeline_desc_t){
		.vs = vs,
		.fs = fs,
		.layout = {
			.location[0] = { .type = ATTRIBUTE_TYPE_FLOAT3, .offset = offsetof(terrain_vertex_t, position), },
			.location[1] = { .type = ATTRIBUTE_TYPE_FLOAT3, .offset = offsetof(terrain_vertex_t, color),    },
			.stride = sizeof(terrain_vertex_t),
		},
		.uniforms = {
			.location[0] = { .type = UNIFORM_TYPE_MAT4, .name = "u_model",      },
			.location[1] = { .type = UNIFORM_TYPE_MAT4, .name = "u_view",       },
			.location[2] = { .type = UNIFORM_TYPE_MAT4, .name = "u_projection", },
		},
		.depth_test = true,
		.culling = true,
	}, &terrain->pipeline);

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

	terrain_t *result = calloc(1, sizeof(terrain_t));

	terrain_init_pipeline(result);
	terrain_init_mesh(result);

	terrain_resize(result, desc->size);

	// using glm_vec3_copy here makes the compiler complain about const variables
	result->position[0] = desc->position[0];
	result->position[1] = desc->position[1];
	result->position[2] = desc->position[2];

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
	free(terrain);
}

void terrain_draw(camera_t *camera, terrain_t *terrain)
{
	HE_ASSERT(terrain != NULL, "Cannot draw NULL terrain");
	HE_ASSERT(camera != NULL, "Cannot draw terrain with NULL camera");

	pipeline_t *last_pip = pipeline_bind(terrain->pipeline);

	// calculate the cordinate system
	mat4 model = GLM_MAT4_IDENTITY_INIT;
	glm_translate(model, terrain->position);

	mat4 view  = GLM_MAT4_IDENTITY_INIT;
	camera_create_view_matrix(camera, view);

	pipeline_set_uniform_mat4(terrain->pipeline, 0, model);
	pipeline_set_uniform_mat4(terrain->pipeline, 1, view);
	pipeline_set_uniform_mat4(terrain->pipeline, 2, camera->projection);

	// draw the terrain
	mesh_draw(terrain->mesh);

	// restore previous state
	pipeline_bind(last_pip);
}

#define size_x (20)
#define size_z (20)

void terrain_resize(terrain_t *terrain, uvec2 size)
{
	HE_ASSERT(terrain != NULL, "Cannot resize NULL");
	HE_ASSERT(size.w > 0 || size.h > 0, "Invalid terrain size");

	terrain_vertex_t vertices[size_x * size_z];
	int indices[(size_x - 1) * (size_z - 1) * 6];

	// thanks brackeys
	int i = 0;
	for (int z = 0; z < size_z; z++)
	{
		for (int x = 0; x < size_x; x++)
		{
			vertices[i].position[0] = (float)x - (size_x / 2.0f);
			vertices[i].position[1] = 0;
			vertices[i].position[2] = (float)z - (size_z / 2.0f);

			vertices[i].color[0] = ((float)x + 1.0f) / size_x;
			vertices[i].color[1] = ((float)x + 1) * ((float)z + 1) / (size_x * size_z);
			vertices[i].color[2] = ((float)z + 1.0f) / size_z;

			i++;
		}
	}

	int vertex = 0;
	int index = 0;
	for (int z = 0; z < size_z - 1; z++)
	{
		for (int x = 0; x < size_x - 1; x++)
		{
			indices[index + 0] = vertex;
			indices[index + 1] = vertex + (size_x - 1) + 1;
			indices[index + 2] = vertex + 1;
			indices[index + 3] = vertex + 1;
			indices[index + 4] = vertex + (size_x - 1) + 1;
			indices[index + 5] = vertex + (size_x - 1) + 2;

			vertex++;
			index += 6;
		}
		vertex++;
	}

	mesh_set_data(terrain->mesh, &(mesh_desc_t){
		.vertices = vertices,
		.vertices_size = sizeof(vertices),
		.indices = indices,
		.indices_size = sizeof(indices),
		.index_count = (size_x - 1) * (size_z - 1) * 6,
	});
}
