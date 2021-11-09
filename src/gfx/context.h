#ifndef __gfx_context_h__
#define __gfx_context_h__

#include <stdbool.h>

#include "buffer.h"
#include "image.h"
#include "pipeline.h"

// it must be done; c does not allow circular dependencies
typedef struct window_t window_t;

#define CONTEXT_DEFAULT__ (NULL)

typedef struct context_desc_t
{
	window_t *window;
	bool debug;
} context_desc_t;

typedef struct context_t
{
	window_t *window;

	buffer_t *cur_buffers[BUFFER_TYPE_COUNT__];
	image_t *cur_images[IMAGE_MAX_BINDINGS__];
	pipeline_t *cur_pipeline;
	
	uint32_t resize_cb_id;
} context_t;

void context_init(const context_desc_t *desc, context_t **context);
context_t *context_create(const context_desc_t *desc);
void context_free(context_t *context);

context_t *context_bind(context_t *context);
context_t *context_get_bound(void);

#endif /* __gfx_context_h__ */
