#ifndef __components_mesh_h__
#define __components_mesh_h__

#include <stdbool.h>
#include <stddef.h>

#include "buffer.h"

typedef struct mesh_desc_t
{
	bool dynamic;

	void *vertices;
	size_t vertices_size;
	size_t vertex_count;

	int *indices;
	size_t indices_size;
	size_t index_count;
} mesh_desc_t;

typedef struct mesh_t
{
	bool dynamic;

	buffer_t *vertices;
	buffer_t *indices;

	size_t vertex_count;
	size_t index_count;
} mesh_t;

void mesh_init(const mesh_desc_t *desc, mesh_t **mesh);
mesh_t *mesh_create(const mesh_desc_t *desc);
void mesh_free(mesh_t *mesh);

void mesh_set_data(mesh_t *mesh, const mesh_desc_t *desc);

void mesh_draw(mesh_t *mesh);

#endif /* __components_mesh_h__ */
