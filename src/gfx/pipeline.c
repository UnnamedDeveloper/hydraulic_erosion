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

static GLint get_attrib_component_count(pipeline_attrib_type_t type)
{
	HE_ASSERT(ATTRIBUTE_TYPE_NONE < type < ATTRIBUTE_TYPE_COUNT__, "Invalid attribute type");

	switch (type)
	{
	default:
	case ATTRIBUTE_TYPE_CHAR:
	case ATTRIBUTE_TYPE_BOOL:
	case ATTRIBUTE_TYPE_FLOAT:
	case ATTRIBUTE_TYPE_INT:
		return 1;
	case ATTRIBUTE_TYPE_FLOAT2:
	case ATTRIBUTE_TYPE_INT2:
		return 2;
	case ATTRIBUTE_TYPE_FLOAT3:
	case ATTRIBUTE_TYPE_INT3:
		return 3;
	case ATTRIBUTE_TYPE_FLOAT4:
	case ATTRIBUTE_TYPE_INT4:
		return 4;
	};
}

static GLenum get_attrib_gl_type(pipeline_attrib_type_t type)
{
	HE_ASSERT(ATTRIBUTE_TYPE_NONE < type < ATTRIBUTE_TYPE_COUNT__, "Invalid attribute type");

	switch (type)
	{
	case ATTRIBUTE_TYPE_CHAR:
		return GL_BYTE;
	case ATTRIBUTE_TYPE_BOOL:
		return GL_BYTE;
	case ATTRIBUTE_TYPE_INT:
	case ATTRIBUTE_TYPE_INT2:
	case ATTRIBUTE_TYPE_INT3:
	case ATTRIBUTE_TYPE_INT4:
		return GL_INT;
	default:
	case ATTRIBUTE_TYPE_FLOAT:
	case ATTRIBUTE_TYPE_FLOAT2:
	case ATTRIBUTE_TYPE_FLOAT3:
	case ATTRIBUTE_TYPE_FLOAT4:
		return GL_FLOAT;
	};
}

bool shader_init(const shader_desc_t *desc, shader_t **shader)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(shader != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A shader description is required");
	HE_ASSERT(desc->source != NULL, "Shader source is required");
	HE_ASSERT(0 <= desc->type < SHADER_TYPE_COUNT__, "Invalid shader typInvalid shader type");

	shader_t *result = calloc(1, sizeof(shader_t));

	result->id = glCreateShader(get_gl_shader_type(desc->type));
	result->type = desc->type;

	glShaderSource(result->id, 1, &desc->source, NULL);
	glCompileShader(result->id);

	GLint success;
	glGetShaderiv(result->id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
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

	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");

	glDeleteShader(shader->id);
	free(shader);
}

bool pipeline_init(const pipeline_desc_t *desc, pipeline_t **pipeline)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(pipeline != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A pipeline description is required");
	HE_ASSERT(desc->vs != NULL, "A vertex shader is required");
	HE_ASSERT(desc->fs != NULL, "A fragment shader is required");
	HE_ASSERT(desc->layout.location[0].type != ATTRIBUTE_TYPE_NONE, "Pipeline must have at least one attribute");

	pipeline_t *result = calloc(1, sizeof(pipeline_t));

	// create pipeline
	result->id = glCreateProgram();
	result->layout = desc->layout;
	result->primitive_type = desc->primitive_type;

	glAttachShader(result->id, desc->vs->id);
	glAttachShader(result->id, desc->fs->id);
	glLinkProgram(result->id);

	GLint success;
	glGetProgramiv(result->id, GL_LINK_STATUS, &success);
	if (!success)
	{
		pipeline_free(result);
		return false;
	}

	// create uniforms
	for (int i = 0; i < PIPELINE_MAX_UNIFORMS__; i++)
	{
		pipeline_uniform_type_t type = desc->uniforms.location[i].type;
		if (type == UNIFORM_TYPE_NONE) break;

		GLint loc = glGetUniformLocation(result->id, desc->uniforms.location[i].name);
		result->uniforms[i].gl_location = loc;
		result->uniforms[i].type = type;
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
	HE_ASSERT(ctx != NULL, "A bound context is required");

	if (ctx->cur_pipeline == pipeline)
		pipeline_bind(NULL);

	free(pipeline);
}

pipeline_t *pipeline_bind(pipeline_t *pipeline)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");

	if (pipeline != NULL)
	{
		glUseProgram(pipeline->id);

		// TODO: Disable previous pipeline attributes
		for (int i = 0; i < PIPELINE_MAX_ATTRIBS__; i++)
		{
			pipeline_attrib_type_t type = pipeline->layout.location[i].type;
			if (type == ATTRIBUTE_TYPE_NONE) break;

			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,
				get_attrib_component_count(type),
				get_attrib_gl_type(type),
				pipeline->layout.location[i].normalized,
				pipeline->layout.stride,
				(void *)pipeline->layout.location[i].offset);
		}
	}
	else
	{
		glUseProgram(PIPELINE_DEFAULT__);
	}

	pipeline_t *last_pip = ctx->cur_pipeline;
	ctx->cur_pipeline = pipeline;
	return last_pip;
}

void pipeline_set_uniform(pipeline_t *pipeline, int location, void *data)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(pipeline != NULL, "Cannot set uniform of NULL");
	HE_ASSERT(0 <= location < PIPELINE_MAX_UNIFORMS__, "Cannot set uniform outside pipeline bounds");

	pipeline_t *last_pip = pipeline_bind(pipeline);

	switch (pipeline->uniforms[location].type)
	{
	case UNIFORM_TYPE_MAT4:
		glUniformMatrix4fv(pipeline->uniforms[location].gl_location, 1, false, data);
		break;
	};

	pipeline_bind(last_pip);
}
