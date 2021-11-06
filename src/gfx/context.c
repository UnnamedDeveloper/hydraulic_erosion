#include "context.h"

#include <stdbool.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "debug/assert.h"
#include "window.h"

static bool glad_loaded = false;

static context_t *cur_context = NULL;

void context_init(const context_desc_t *desc, context_t **context)
{
	HE_ASSERT(desc != NULL);
	HE_ASSERT(context != NULL);

	context_t *result = calloc(1, sizeof(context_t));

	result->window = desc->window;

	if (!glad_loaded)
	{
		// bind current context
		context_t *last_ctx = context_bind(result);

		// init glad
		bool status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HE_ASSERT(status);
		glad_loaded = status;

		// make sure nothing was messed up
		context_bind(last_ctx);
	}

	*context = result;
}

context_t *context_create(const context_desc_t *desc)
{
	context_t *context;
	context_init(desc, &context);
	return context;
}

void context_free(context_t *context)
{
	// unbind context as current
	if (cur_context == context)
		context_bind(NULL);
	
	free(context);
}

context_t *context_bind(context_t *context)
{
	if (context != NULL)
	{
		glfwMakeContextCurrent(context->window->glfw_window);
	}
	else
	{
		glfwMakeContextCurrent(NULL);
	}

	context_t *last_ctx = cur_context;
	cur_context = context;
	return last_ctx;
}
