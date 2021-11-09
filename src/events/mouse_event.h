#ifndef __events_mouse_event_h__
#define __events_mouse_event_h__

#include <cglm/cglm.h>

typedef struct mouse_move_event_t
{
	vec2 position;
	vec2 offset;
} mouse_move_event_t;

typedef struct mouse_scroll_event_t
{
	vec2 offset;
} mouse_scroll_event_t;

typedef struct mouse_press_event_t
{
	int button;
} mouse_press_event_t;

typedef struct mouse_release_event_t
{
	int button;
} mouse_release_event_t;

#endif /* __events_mouse_event_h__ */
