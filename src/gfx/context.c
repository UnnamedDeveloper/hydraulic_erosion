#include "context.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "debug/assert.h"
#include "events/window_event.h"
#include "window.h"

static bool glad_loaded = false;

static context_t *cur_context = NULL;

static bool on_window_resize(event_bus_t *bus, bool handled, void *user_pointer, window_resize_event_t *event)
{
	context_t *last_ctx = context_bind((context_t *)user_pointer);
	renderer_set_viewport(0, 0, event->size.w, event->size.h);
	context_bind(last_ctx);
	return true;
}

static void gl_debug_callback(GLenum source, GLenum type, uint32_t id, GLenum severity, GLsizei length, const char *msg, const void *user)
{
	printf("OpenGL Debug Message (");
	printf("Source: ");
	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             printf("API"); break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   printf("Window System"); break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: printf("Shader Compiler"); break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     printf("Third Party"); break;
	case GL_DEBUG_SOURCE_APPLICATION:     printf("Application"); break;
	case GL_DEBUG_SOURCE_OTHER:           printf("Other"); break;
	};

	printf(", Type: ");
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:                printf("ERROR"); break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:  printf("DEPRECATED BEHAVIOR"); break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:   printf("UNDEFINED BEHAVIOR"); break;
	case GL_DEBUG_TYPE_PORTABILITY:          printf("PORTABILITY"); break;
	case GL_DEBUG_TYPE_PERFORMANCE:          printf("PERFORMANCE"); break;
	case GL_DEBUG_TYPE_MARKER:               printf("MARKER"); break;
	case GL_DEBUG_TYPE_PUSH_GROUP:           printf("PUSH GROUP"); break;
	case GL_DEBUG_TYPE_POP_GROUP:            printf("POP GROUP"); break;
	case GL_DEBUG_TYPE_OTHER:                printf("OTHER"); break;
	};

	printf(", Severity: ");
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         printf("HIGH"); break;
	case GL_DEBUG_SEVERITY_MEDIUM:       printf("Medium"); break;
	case GL_DEBUG_SEVERITY_LOW:          printf("Low"); break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: printf("Notification"); break;
	};

	printf(")\n%s\n\n", msg);
}

void context_init(const context_desc_t *desc, context_t **context)
{
	HE_ASSERT(context != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A context description is required");

	context_t *result = malloc(sizeof(context_t));
	if (result != NULL) memset(result, 0, sizeof(context_t));

	result->window = desc->window;

	// bind current context
	context_t *last_ctx = context_bind(result);

	if (!glad_loaded)
	{
		bool status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HE_ASSERT(status, "Failed to initialize GLAD");
		glad_loaded = status;
	}

#ifdef GL_ARB_debug_output
	int gl_flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &gl_flags);

	bool gl_debug_context = (gl_flags & GL_CONTEXT_FLAG_DEBUG_BIT) != 0;
	if (glad_loaded && desc->debug && GLAD_GL_ARB_debug_output && gl_debug_context)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)gl_debug_callback, NULL);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	}
#endif

	// setup events
	result->resize_cb_id = event_subscribe(desc->window->event_bus,
		EVENT_TYPE_WINDOW_RESIZE,
		EVENT_LAYER_APP,
		result,
		(event_callback_fn_t)on_window_resize);

	// restore previous context
	context_bind(last_ctx);

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

	// unsubscribe from events
	event_unsubscribe(context->window->event_bus,
		EVENT_TYPE_WINDOW_RESIZE,
		EVENT_LAYER_APP,
		context->resize_cb_id);

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
			for (int i = 0; i < IMAGE_MAX_BINDINGS__; i++)
			{
				image_bind(i, cur_context->cur_images[i]);
			}
			pipeline_bind(cur_context->cur_pipeline);
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
