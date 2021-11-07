#ifndef __gfx_renderer_h__
#define __gfx_renderer_h__

#include <cglm/cglm.h>
#include <glad/glad.h>

typedef enum primitive_type_t
{
	PRIMITIVE_TYPE_DEFAULT__,
	PRIMITIVE_TYPE_POINTS,
	PRIMITIVE_TYPE_LINES,
	PRIMITIVE_TYPE_LINE_STRIP,
	PRIMITIVE_TYPE_TRIANGLES,
	PRIMITIVE_TYPE_TRIANGLE_STRIP,
	PRIMITIVE_TYPE_COUNT__,
} primitive_type_t;

typedef struct cmd_clear_desc_t
{
	vec4 color;
	float depth;
} cmd_clear_desc_t;

void renderer_clear(cmd_clear_desc_t *cmd);

void renderer_draw_indexed(uint32_t count);

#endif /* __gfx_renderer_h__ */
