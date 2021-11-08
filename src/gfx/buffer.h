#ifndef __gfx_buffer_h__
#define __gfx_buffer_h__

#include <stddef.h>

#include <glad/glad.h>

#define BUFFER_DEFAULT__ (0)

typedef enum buffer_type_t
{
	BUFFER_TYPE_VERTEX,
	BUFFER_TYPE_INDEX,
	BUFFER_TYPE_COUNT__,
} buffer_type_t;

typedef enum buffer_usage_t
{
	BUFFER_USAGE_STATIC,
	BUFFER_USAGE_DYNAMIC,
	BUFFER_USAGE_COUNT__,
} buffer_usage_t;

typedef struct buffer_desc_t
{
	size_t size;
	void *data;
	buffer_type_t type;
	buffer_usage_t usage;
} buffer_desc_t;

typedef struct buffer_t
{
	GLuint id;
	buffer_type_t type;
	buffer_usage_t usage;
	size_t size;
	void *data;
} buffer_t;

void buffer_init(const buffer_desc_t *desc, buffer_t **buffer);
buffer_t *buffer_create(const buffer_desc_t *desc);
void buffer_free(buffer_t *buffer);

buffer_t *buffer_bind(buffer_t *buffer);
buffer_t *buffer_bind_to(buffer_type_t to, buffer_t *buffer);

void buffer_set_data(buffer_t *buffer, size_t size, void *data);

#endif /* __gfx_buffer_h__ */
