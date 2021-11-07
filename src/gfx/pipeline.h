#ifndef __gfx_pipeline_h__
#define __gfx_pipeline_h__

#include <stdbool.h>
#include <stddef.h>

#include <glad/glad.h>

typedef enum shader_type_t
{
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,
	SHADER_TYPE_COUNT__,
} shader_type_t;

typedef struct shader_desc_t
{
	const char *source;
	shader_type_t type;
} shader_desc_t;

typedef struct shader_t
{
	GLuint id;
	shader_type_t type;
} shader_t;

bool shader_init(const shader_desc_t *desc, shader_t **shader);
shader_t *shader_create(const shader_desc_t *desc);
void shader_free(shader_t *shader);

#define PIPELINE_DEFAULT__ (0)

#define PIPELINE_MAX_ATTRIBS__ (GL_MAX_VERTEX_ATTRIBS)

typedef enum pipeline_attrib_type_t
{
	ATTRIBUTE_TYPE_NONE,
	ATTRIBUTE_TYPE_CHAR,
	ATTRIBUTE_TYPE_BOOL,
	ATTRIBUTE_TYPE_INT,
	ATTRIBUTE_TYPE_INT2,
	ATTRIBUTE_TYPE_INT3,
	ATTRIBUTE_TYPE_INT4,
	ATTRIBUTE_TYPE_FLOAT,
	ATTRIBUTE_TYPE_FLOAT2,
	ATTRIBUTE_TYPE_FLOAT3,
	ATTRIBUTE_TYPE_FLOAT4,
	ATTRIBUTE_TYPE_COUNT__,
} pipeline_attrib_type_t;

typedef struct pipeline_attrib_desc_t
{
	pipeline_attrib_type_t type;
	bool normalized;
	uint32_t offset;
} pipeline_attrib_desc_t;

typedef struct pipeline_layout_desc_t
{
	pipeline_attrib_desc_t location[PIPELINE_MAX_ATTRIBS__];
	size_t stride;
} pipeline_layout_desc_t;

typedef struct pipeline_desc_t
{
	shader_t *vs;
	shader_t *fs;
	pipeline_layout_desc_t layout;
} pipeline_desc_t;

typedef struct pipeline_t
{
	GLuint id;
	pipeline_layout_desc_t layout;
} pipeline_t;

bool pipeline_init(const pipeline_desc_t *desc, pipeline_t **pipeline);
pipeline_t *pipeline_create(const pipeline_desc_t *desc);
void pipeline_free(pipeline_t *pipeline);

pipeline_t *pipeline_bind(pipeline_t *pipeline);

#endif /* __gfx_pipeline_h__ */
