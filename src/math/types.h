#ifndef __math_types_h__
#define __math_types_h__

#include <stdint.h>

typedef struct uvec2
{
	union
	{
		uint32_t size[2];
		struct { uint32_t width, height; };
		struct { uint32_t w, h; };
		struct { uint32_t x, y; };
	};
} uvec2;

typedef struct ivec2
{
	union
	{
		int size[2];
		struct { int width, height; };
		struct { int w, h; };
		struct { int x, y; };
	};
} ivec2;

#endif /* __math_types_h__ */