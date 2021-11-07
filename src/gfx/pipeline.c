#include "pipeline.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug/assert.h"
#include "context.h"

static GLenum get_gl_shader_type(shader_type_t type)
{
	switch (type)
	{
	default:
	case SHADER_TYPE_VERTEX:
		return GL_VERTEX_SHADER;
	case SHADER_TYPE_FRAGMENT:
		return GL_FRAGMENT_SHADER;
	};
}

bool shader_init(const shader_desc_t *desc, shader_t **shader)
{
	HE_ASSERT(shader != NULL);
	HE_ASSERT(desc != NULL);
	HE_ASSERT(desc->source != NULL);
	HE_ASSERT(0 <= desc->type < SHADER_TYPE_COUNT__);

	shader_t *result = calloc(1, sizeof(shader_t));

	result->id = glCreateShader(get_gl_shader_type(desc->type));
	result->type = desc->type;

	glShaderSource(result->id, 1, &desc->source, NULL);
	glCompileShader(result->id);

	GLint success;
	glGetShaderiv(result->id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		printf("Failed to compile shader\n");
		shader_free(result);
		return false;
	}

	*shader = result;
	return true;
}

shader_t *shader_create(const shader_desc_t *desc)
{
	shader_t *shader;
	if (!shader_init(desc, &shader)) return NULL;
	return shader;
}

void shader_free(shader_t *shader)
{
	if (shader == NULL) return;

	glDeleteShader(shader->id);
	free(shader);
}

bool pipeline_init(const pipeline_desc_t *desc, pipeline_t **pipeline)
{
	HE_ASSERT(pipeline != NULL);
	HE_ASSERT(desc != NULL);
	HE_ASSERT(desc->vs != NULL);
	HE_ASSERT(desc->fs != NULL);

	pipeline_t *result = calloc(1, sizeof(pipeline_t));

	result->id = glCreateProgram();

	glAttachShader(result->id, desc->vs->id);
	glAttachShader(result->id, desc->fs->id);
	glLinkProgram(result->id);

	GLint success;
	glGetProgramiv(result->id, GL_LINK_STATUS, &success);
	if (!success)
	{
		printf("Failed to link pipeline\n");
		pipeline_free(result);
		return false;
	}

	*pipeline = result;
	return true;
}

pipeline_t *pipeline_create(const pipeline_desc_t *desc)
{
	pipeline_t *pipeline;
	if (!pipeline_init(desc, &pipeline)) return NULL;
	return pipeline;
}

void pipeline_free(pipeline_t *pipeline)
{
	if (pipeline == NULL) return;

	context_t *ctx = context_get_bound();
	if (ctx->cur_pipeline == pipeline)
		pipeline_bind(NULL);

	free(pipeline);
}

pipeline_t *pipeline_bind(pipeline_t *pipeline)
{
	if (pipeline != NULL)
	{
		glUseProgram(pipeline->id);
	}
	else
	{
		glUseProgram(PIPELINE_DEFAULT__);
	}

	context_t *ctx = context_get_bound();
	pipeline_t *last_pip = ctx->cur_pipeline;
	ctx->cur_pipeline = pipeline;
	return last_pip;
}
