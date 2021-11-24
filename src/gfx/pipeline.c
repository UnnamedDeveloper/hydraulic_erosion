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

static void validate_uniform_function(pipeline_t *pipeline, int location, pipeline_uniform_type_t type)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(pipeline != NULL, "Cannot set uniform of NULL");
	HE_ASSERT(type == pipeline->uniforms[location].type, "Cannot set uniform to value of different type");
	HE_ASSERT(0 <= location < PIPELINE_MAX_UNIFORMS__, "Cannot set uniform outside pipeline bounds");
}

bool shader_init(const shader_desc_t *desc, shader_t **shader)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(shader != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A shader description is required");
	HE_ASSERT(desc->source != NULL, "Shader source is required");
	HE_ASSERT(0 <= desc->type < SHADER_TYPE_COUNT__, "Invalid shader typInvalid shader type");

	shader_t *result = malloc(sizeof(shader_t));

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

	pipeline_t *result = malloc(sizeof(pipeline_t));

	// create pipeline
	result->id = glCreateProgram();
	result->layout = desc->layout;
	result->primitive_type = desc->primitive_type;
	result->depth_test = desc->depth_test;
	result->culling = desc->culling;
	result->wireframe = desc->wireframe;

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

	// bind images to correct units
	pipeline_t *last_pip = pipeline_bind(result);
	for (int i = 0; i < IMAGE_MAX_BINDINGS__; i++)
	{
		if (desc->images.location[i].type == IMAGE_TYPE_NONE) break;
		glUniform1i(glGetUniformLocation(result->id, desc->images.location[i].name), i);
	}
	pipeline_bind(last_pip);

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

		// enable gl functions
		if (pipeline->depth_test)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		if (pipeline->culling)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}

		glPolygonMode(GL_FRONT_AND_BACK, pipeline->wireframe ? GL_LINE : GL_FILL);

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

void pipeline_set_uniformf(pipeline_t *pipeline, int location, float data)
{
	validate_uniform_function(pipeline, location, UNIFORM_TYPE_FLOAT);
	pipeline_t *last_pip = pipeline_bind(pipeline);
	glUniform1f(pipeline->uniforms[location].gl_location, data);
	pipeline_bind(last_pip);
}

void pipeline_set_uniformf3(pipeline_t *pipeline, int location, vec3 data)
{
	validate_uniform_function(pipeline, location, UNIFORM_TYPE_FLOAT3);
	pipeline_t *last_pip = pipeline_bind(pipeline);
	glUniform3f(pipeline->uniforms[location].gl_location, data[0], data[1], data[2]);
	pipeline_bind(last_pip);
}

void pipeline_set_uniform_mat4(pipeline_t *pipeline, int location, mat4 data)
{
	validate_uniform_function(pipeline, location, UNIFORM_TYPE_MAT4);
	pipeline_t *last_pip = pipeline_bind(pipeline);
	glUniformMatrix4fv(pipeline->uniforms[location].gl_location, 1, false, (float *)data);
	pipeline_bind(last_pip);
}
