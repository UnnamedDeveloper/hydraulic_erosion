#ifndef __gfx_context_h__
#define __gfx_context_h__

#include "buffer.h"

// it must be done; c does not allow circular dependencies
typedef struct window_t window_t;

typedef struct context_desc_t
{
	window_t *window;
} context_desc_t;

typedef struct context_t
{
	window_t *window;

	buffer_t *cur_buffers[BUFFER_TYPE_COUNT__];
} context_t;

void context_init(const context_desc_t *desc, context_t **context);
context_t *context_create(const context_desc_t *desc);
void context_free(context_t *context);

context_t *context_bind(context_t *context);
context_t *context_get_bound(void);

#endif /* __gfx_context_h__ */
