#ifndef __gfx_image_h__
#define __gfx_image_h__

#include <glad/glad.h>

#include "math/types.h"

// this might not apply. this is the max image binding count, not how many
// images the fragment shader can access.
#define IMAGE_MAX_BINDINGS__ (GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS)

typedef enum image_type_t
{
	IMAGE_TYPE_NONE,
	IMAGE_TYPE_2D,
	IMAGE_TYPE_COUNT__,
} image_type_t;

typedef enum image_format_t
{
	IMAGE_FORMAT_NONE,
	IMAGE_FORMAT_RGB8,
	IMAGE_FORMAT_RGBA8,
	IMAGE_FORMAT_COUNT__,
} image_format_t;

typedef enum image_wrap_t
{
	IMAGE_WRAP_REPEAT,
	IMAGE_WRAP_COUNT__,
} image_wrap_t;

typedef enum image_filter_t
{
	IMAGE_FILTER_LINEAR,
	IMAGE_FILTER_COUNT__,
} image_filter_t;

typedef struct image_desc_t
{
	image_type_t type;
	image_format_t format;
	image_wrap_t wrap_s;
	image_wrap_t wrap_t;
	image_filter_t min;
	image_filter_t mag;

	uvec2 size;
	void *data;
} image_desc_t;

typedef struct image_t
{
	GLuint id;
	image_type_t type;
} image_t;

void image_init(const image_desc_t *desc, image_t **image);
image_t *image_create(const image_desc_t *desc);
void image_free(image_t *image);

image_t *image_bind(uint8_t location, image_t *image);

#endif /* __gfx_image_h__ */
