#include "renderer.h"

#include "debug/assert.h"
#include "context.h"

static GLenum get_gl_primitive_type(primitive_type_t type)
{
	switch (type)
	{
	case PRIMITIVE_TYPE_POINTS:
		return GL_POINTS;
	case PRIMITIVE_TYPE_LINES:
		return GL_LINES;
	case PRIMITIVE_TYPE_LINE_STRIP:
		return GL_LINE_STRIP;
	default:
	case PRIMITIVE_TYPE_DEFAULT__:
	case PRIMITIVE_TYPE_TRIANGLES:
		return GL_TRIANGLES;
	case PRIMITIVE_TYPE_TRIANGLE_STRIP:
		return GL_TRIANGLE_STRIP;
	};
}

void renderer_clear(cmd_clear_desc_t *cmd)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	glClearColor(cmd->color[0], cmd->color[1], cmd->color[2], cmd->color[3]);
	glad_glClearDepth(cmd->depth);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_draw_indexed(uint32_t count)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(ctx->cur_buffers[BUFFER_TYPE_INDEX] != NULL, "A bound index buffer is required for indexed drawing");

	glDrawElements(get_gl_primitive_type(ctx->cur_pipeline->primitive_type),
		count,
		GL_UNSIGNED_INT,
		0);
}
