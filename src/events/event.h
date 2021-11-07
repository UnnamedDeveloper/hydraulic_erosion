#ifndef __events_event_h__
#define __events_event_h__

#include <stdint.h>

#define EVENT_MAX_CALLBACKS__ (32)

typedef enum event_type_t
{
	EVENT_TYPE_WINDOW_CLOSE,
	EVENT_TYPE_WINDOW_RESIZE,
	EVENT_TYPE_COUNT__,
} event_type_t;

typedef struct event_t event_t;

typedef struct event_bus_t event_bus_t;
typedef void(*event_callback_fn_t)(event_bus_t *, event_t *);

typedef struct event_bus_desc_t
{
	void *user_pointer;
} event_bus_desc_t;

typedef struct event_bus_t
{
	void *user_pointer;
	event_callback_fn_t callbacks[EVENT_TYPE_COUNT__][EVENT_MAX_CALLBACKS__];
	uint32_t callback_indices[EVENT_TYPE_COUNT__];
} event_bus_t;

void event_bus_init(const event_bus_desc_t *desc, event_bus_t **event_bus);
event_bus_t *event_bus_create(const event_bus_desc_t *desc);
void event_bus_free(event_bus_t *event_bus);

void event_publish(event_bus_t *bus, event_type_t type, event_t *event);
void event_subscribe(event_bus_t *bus, event_type_t type, event_callback_fn_t callback);
void event_unsubscribe(event_bus_t *bus, event_type_t type, event_callback_fn_t callback);

#endif /* __events_event_h__ */
