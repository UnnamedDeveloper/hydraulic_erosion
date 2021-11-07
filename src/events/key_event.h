#ifndef __events_key_event_h__
#define __events_key_event_h__

#include <wchar.h>

typedef struct char_type_event_t
{
	char c;
	wchar_t wide;
	int keycode;
} char_type_event_t;

typedef struct key_press_event_t
{
	int key;
} key_press_event_t;

typedef struct key_release_event_t
{
	int key;
} key_release_event_t;

#endif /* __events_key_event_h__ */
