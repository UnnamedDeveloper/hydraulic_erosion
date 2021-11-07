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
	if (context == NULL) return;

	// unbind context as current
	if (cur_context == context)
		context_bind(NULL);

	free(context);
}

context_t *context_bind(context_t *context)
{
	// bind context
	context_t *last_ctx = cur_context;
	cur_context = context;

	if (context != NULL)
	{
		glfwMakeContextCurrent(context->window->glfw_window);

		if (glad_loaded)
		{
			// bind context objects
			for (int i = 0; i < BUFFER_TYPE_COUNT__; i++)
			{
				buffer_bind_to((buffer_type_t)i, cur_context->cur_buffers[i]);
			}
		}
	}
	else
	{
		glfwMakeContextCurrent(CONTEXT_DEFAULT__);
	}

	return last_ctx;
}

context_t *context_get_bound(void)
{
	return cur_context;
}
