#include "mesh.h"

#include <stdlib.h>

#include "debug/assert.h"
#include "renderer.h"

void mesh_init(const mesh_desc_t *desc, mesh_t **mesh)
{
	HE_ASSERT(mesh != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A mesh description is required");

	mesh_t *result = malloc(sizeof(mesh_t));

	result->dynamic = desc->dynamic;
	result->vertex_count = desc->vertex_count;
	result->index_count = desc->index_count;

	buffer_init(&(buffer_desc_t){
		.type = BUFFER_TYPE_VERTEX,
		.usage = desc->dynamic ? BUFFER_USAGE_DYNAMIC : BUFFER_USAGE_STATIC,
		.data = desc->vertices,
		.size = desc->vertices_size,
	}, &result->vertices);

	buffer_init(&(buffer_desc_t){
		.type = BUFFER_TYPE_INDEX,
		.usage = desc->dynamic ? BUFFER_USAGE_DYNAMIC : BUFFER_USAGE_STATIC,
		.data = desc->indices,
		.size = desc->indices_size,
	}, &result->indices);

	*mesh = result;
}

mesh_t *mesh_create(const mesh_desc_t *desc)
{
	mesh_t *mesh;
	mesh_init(desc, &mesh);
	return mesh;
}

void mesh_free(mesh_t *mesh)
{
	if (mesh == NULL) return;

	buffer_free(mesh->vertices);
	buffer_free(mesh->indices);
	free(mesh);
}

void mesh_set_data(mesh_t *mesh, const mesh_desc_t *desc)
{
	HE_ASSERT(mesh != NULL, "Cannot set data of NULL");
	HE_ASSERT(mesh->dynamic, "Cannot edit data of static mesh");

	buffer_set_data(mesh->vertices, desc->vertices_size, desc->vertices);
	buffer_set_data(mesh->indices, desc->indices_size, desc->indices);
	mesh->vertex_count = desc->vertex_count;
	mesh->index_count = desc->index_count;
}

void mesh_draw(mesh_t *mesh)
{
	HE_ASSERT(mesh != NULL, "Cannot draw NULL");
	HE_ASSERT(mesh->vertices != NULL, "Cannot draw mesh without any vertices");

	buffer_t *last_vtx_buf = buffer_bind(mesh->vertices);
	if (mesh->indices != NULL)
	{
		buffer_t *last_idx_buf = buffer_bind(mesh->indices);
		renderer_draw_indexed(mesh->index_count);
		buffer_bind(last_idx_buf);
	}
	else
	{
		renderer_draw(mesh->vertex_count, 0);
	}
	buffer_bind(last_vtx_buf);
}
