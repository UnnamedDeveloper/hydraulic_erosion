#include "event.h"

#include <stdlib.h>

#include "debug/assert.h"

void event_bus_init(const event_bus_desc_t *desc, event_bus_t **event_bus)
{
	HE_ASSERT(event_bus != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "An event bus description is required");

	event_bus_t *result = calloc(1, sizeof(event_bus_t));

	*event_bus = result;
}

event_bus_t *event_bus_create(const event_bus_desc_t *desc)
{
	event_bus_t *bus;
	event_bus_init(desc, &bus);
	return bus;
}

void event_bus_free(event_bus_t *event_bus)
{
	free(event_bus);
}

void event_publish(event_bus_t *bus, event_type_t type, event_t *event)
{
	HE_ASSERT(bus != NULL, "Cannot publish an event to NULL");
	HE_ASSERT(0 <= type < EVENT_TYPE_COUNT__, "Invalid event type");

	for (int i = 0; i < EVENT_MAX_CALLBACKS__; i++)
	{
		event_callback_fn_t cb = bus->callbacks[type][i];
		void *ptr = bus->user_pointers[type][i];
		if (cb == NULL) continue;
		cb(bus, ptr, event);
	}
}

uint32_t event_subscribe(event_bus_t *bus, event_type_t type, void *user_pointer, event_callback_fn_t callback)
{
	HE_ASSERT(bus != NULL, "Cannot subscribe to NULL");
	HE_ASSERT(0 <= type < EVENT_TYPE_COUNT__, "Invalid event type");

	uint32_t id = EVENT_MAX_CALLBACKS__;
	for (int i = 0; i < EVENT_MAX_CALLBACKS__; i++)
	{
		if (bus->callbacks[type][i] == NULL)
			id = i;
	}

	assert(id != EVENT_MAX_CALLBACKS__, "Event bus callbacks exhausted");

	bus->user_pointers[type][id] = user_pointer;
	bus->callbacks[type][id] = callback;

	return id;
}

void event_unsubscribe(event_bus_t *bus, event_type_t type, uint32_t id)
{
	bus->callbacks[type][id] = NULL;
	bus->user_pointers[type][id] = NULL;
}
