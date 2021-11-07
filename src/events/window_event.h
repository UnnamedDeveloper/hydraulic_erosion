#ifndef __events_window_event_h__
#define __events_window_event_h__

#include "gfx/window.h"
#include "math/types.h"

typedef struct window_close_event_t
{
	window_t *window;
} window_close_event_t;

typedef struct window_resize_event_t
{
	window_t *window;
	uvec2 size;
} window_resize_event_t;

#endif /* __events_window_event_h__ */
