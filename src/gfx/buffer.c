#include "buffer.h"

#include <stdlib.h>

#include "debug/assert.h"
#include "context.h"

static GLenum get_gl_buffer_target(buffer_type_t type)
{
	switch (type)
	{
	default:
	case BUFFER_TYPE_VERTEX:
		return GL_ARRAY_BUFFER;
	case BUFFER_TYPE_INDEX:
		return GL_ELEMENT_ARRAY_BUFFER;
	};
}

void buffer_init(const buffer_desc_t *desc, buffer_t **buffer)
{
	HE_ASSERT(desc != NULL);
	HE_ASSERT(buffer != NULL);

	buffer_t *result = calloc(1, sizeof(buffer_t));

	glCreateBuffers(1, &result->id);
	result->type = desc->type;

	buffer_t *last_buf = buffer_bind(result);
	glBufferData(get_gl_buffer_target(result->type), desc->size, desc->data, GL_STATIC_DRAW);
	buffer_bind(last_buf);

	*buffer = result;
}

buffer_t *buffer_create(const buffer_desc_t *desc)
{
	buffer_t *buffer;
	buffer_init(desc, &buffer);
	return buffer;
}

void buffer_free(buffer_t *buffer)
{
	context_t *ctx = context_get_bound();
	if (ctx->cur_buffers[buffer->type] == buffer)
		buffer_bind(NULL);

	glDeleteBuffers(1, &buffer->id);
	free(buffer);
}

buffer_t *buffer_bind(buffer_t *buffer)
{
	if (buffer != NULL)
	{
		return buffer_bind_to(buffer->type, buffer);
	}
	else
	{
		// cannot do anything here as the type is not known
		return NULL;
	}
}

buffer_t *buffer_bind_to(buffer_type_t to, buffer_t *buffer)
{
	if (buffer != NULL)
	{
		glBindBuffer(get_gl_buffer_target(to), buffer->id);
	}
	else
	{
		glBindBuffer(get_gl_buffer_target(to), 0);
	}

	context_t *ctx = context_get_bound();
	buffer_t *last_buf = ctx->cur_buffers[to];
	ctx->cur_buffers[to] = buffer;
	return last_buf;
}
