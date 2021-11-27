#include "terrain.h"

#include <stdlib.h>

#include "debug/assert.h"
#include "io/file.h"

typedef struct terrain_vertex_t
{
	vec3 position;
	vec3 normal;
} terrain_vertex_t;

static shader_t *create_shader(const char *path, shader_type_t type)
{
	size_t file_size = 0;
	FILE *file = NULL;

	file = file_open(path);

	HE_ASSERT(file != NULL, "Failed to open shader file");

	file_read(file, &file_size, NULL);
	char *source = malloc(file_size + 1);
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
			.location[1] = {.type = ATTRIBUTE_TYPE_FLOAT3, .offset = offsetof(terrain_vertex_t, normal),    },
			.stride = sizeof(terrain_vertex_t),
		},
		.uniforms = {
			.location[0] = {.type = UNIFORM_TYPE_MAT4,   .name = "u_model",      },
			.location[1] = {.type = UNIFORM_TYPE_MAT4,   .name = "u_view",       },
			.location[2] = {.type = UNIFORM_TYPE_MAT4,   .name = "u_projection", },
			.location[3] = {.type = UNIFORM_TYPE_FLOAT3, .name = "u_light_pos",  },
			.location[4] = {.type = UNIFORM_TYPE_FLOAT3, .name = "u_camera_pos", },
		},
		.depth_test = true,
		.culling = true,
	};

	pipeline_init(&desc, &terrain->pipeline);
#ifndef NDEBUG
	shader_t *wireframe_fs = create_shader("res/shaders/terrain_wireframe.fs.glsl", SHADER_TYPE_FRAGMENT);

	desc.wireframe = true,
	desc.fs = wireframe_fs;
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
	HE_ASSERT(desc->elevation != 0, "Elevation of zero will flatten terrain");

	terrain_t *result = malloc(sizeof(terrain_t));

	result->noise_function = desc->noise_function;
	result->seed = desc->seed;
	result->scale_scalar = desc->scale_scalar;
	result->elevation = desc->elevation;
	result->height_map = NULL;

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
	free(terrain->height_map);
	free(terrain);
}

void terrain_draw(camera_t *camera, vec3 light_pos, terrain_t *terrain)
{
	HE_ASSERT(terrain != NULL, "Cannot draw NULL terrain");
	HE_ASSERT(light_pos != NULL, "Cannot draw terrain without a light");
	HE_ASSERT(camera != NULL, "Cannot draw terrain without a camera");

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
	pipeline_set_uniformf3(terrain->pipeline, 3, light_pos);
	pipeline_set_uniformf3(terrain->pipeline, 4, camera->position);

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

void terrain_update_mesh(terrain_t *terrain)
{
	size_t vertex_count = terrain->size.w * terrain->size.h;
	terrain_vertex_t *vertices = malloc(vertex_count * sizeof(terrain_vertex_t));

	size_t index_count = (terrain->size.w - 1) * (terrain->size.h - 1) * 6;
	int *indices = malloc(index_count * sizeof(int));

	// thanks brackeys
	int i = 0;
	for (int z = 0; z < terrain->size.h; z++)
	{
		for (int x = 0; x < terrain->size.w; x++)
		{
			float vx = ((float) x - (terrain->size.w / 2.0f)) * terrain->scale_scalar;
			float vz = ((float) z - (terrain->size.h / 2.0f)) * terrain->scale_scalar;
			vertices[i].position[0] = vx;
			vertices[i].position[1] = terrain_get_height(terrain, x, z) * terrain->elevation;
			vertices[i].position[2] = vz;
			
			glm_vec3_copy(GLM_VEC3_ZERO, vertices[i].normal);

			i++;
		}
	}

	int vertex = 0;
	int index = 0;
	for (int z = 0; z < terrain->size.h - 1; z++)
	{
		for (int x = 0; x < terrain->size.w - 1; x++)
		{
			vec3 p, ba, ca;
			int a, b, c;

			// -- tri 1
			// indices
			a = vertex;
			b = vertex + (terrain->size.w - 1) + 1;
			c = vertex + 1;
			indices[index + 0] = a;
			indices[index + 1] = b;
			indices[index + 2] = c;

			// normals
			glm_vec3_sub(vertices[b].position, vertices[a].position, ba);
			glm_vec3_sub(vertices[c].position, vertices[a].position, ca);
			glm_vec3_cross(ba, ca, p);

			glm_vec3_add(vertices[a].normal, p, vertices[a].normal);
			glm_vec3_add(vertices[b].normal, p, vertices[b].normal);
			glm_vec3_add(vertices[c].normal, p, vertices[c].normal);

			// -- tri 2
			// indices
			a = vertex + 1;
			b = vertex + (terrain->size.w - 1) + 1;
			c = vertex + (terrain->size.w - 1) + 2;
			indices[index + 3] = a;
			indices[index + 4] = b;
			indices[index + 5] = c;

			// normals
			glm_vec3_sub(vertices[b].position, vertices[a].position, ba);
			glm_vec3_sub(vertices[c].position, vertices[a].position, ca);
			glm_vec3_cross(ba, ca, p);

			glm_vec3_add(vertices[a].normal, p, vertices[a].normal);
			glm_vec3_add(vertices[b].normal, p, vertices[b].normal);
			glm_vec3_add(vertices[c].normal, p, vertices[c].normal);

			vertex++;
			index += 6;
		}
		vertex++;
	}

	for (int i = 0; i < vertex_count; i++)
	{
		glm_vec3_normalize(vertices[i].normal);
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

float terrain_get_height(terrain_t *terrain, uint32_t x, uint32_t y)
{
	HE_ASSERT(terrain != NULL, "Cannot get height of NULL");
	HE_ASSERT(x < terrain->size.w, "X coord outside terrain bounds");
	HE_ASSERT(y < terrain->size.h, "X coord outside terrain bounds");

	return terrain->height_map[x + y * terrain->size.w];
}

void terrain_set_height(terrain_t *terrain, uint32_t x, uint32_t y, float v)
{
	HE_ASSERT(terrain != NULL, "Cannot set height of NULL");
	HE_ASSERT(x < terrain->size.w, "X coord outside terrain bounds");
	HE_ASSERT(y < terrain->size.h, "X coord outside terrain bounds");

	terrain->height_map[x + y * terrain->size.w] = v;
}

void terrain_resize(terrain_t *terrain, uvec2 size)
{
	HE_ASSERT(terrain != NULL, "Cannot resize NULL");
	HE_ASSERT(size.w > 0 || size.h > 0, "Invalid terrain size");

	terrain->size = size;

	if (terrain->height_map != NULL) free(terrain->height_map);
	terrain->height_map = malloc(size.w * size.h * sizeof(float));

	for (int x = 0; x < terrain->size.w; x++)
	{
		for (int z = 0; z < terrain->size.h; z++)
		{
			terrain_set_height(terrain, x, z, terrain->noise_function(terrain->seed, (float) x * terrain->scale_scalar, (float) z * terrain->scale_scalar));
		}
	}

	terrain_update_mesh(terrain);
}

void terrain_reset(terrain_t *terrain)
{
	terrain_resize(terrain, terrain->size);
}

uvec2 terrain_get_size(terrain_t *terrain)
{
	return terrain->size;
}
