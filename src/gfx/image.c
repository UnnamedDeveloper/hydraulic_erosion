#include "image.h"

#include <stdlib.h>

#include "debug/assert.h"
#include "context.h"

static GLenum get_gl_image_type(image_type_t type)
{
	switch (type)
	{
	default:
	case IMAGE_TYPE_2D:
		return GL_TEXTURE_2D;
	}
}

static GLenum get_gl_image_wrap(image_wrap_t wrap)
{
	switch (wrap)
	{
	default:
	case IMAGE_WRAP_REPEAT:
		return GL_REPEAT;
	};
}

static GLenum get_gl_image_filter(image_filter_t filter)
{
	switch (filter)
	{
	default:
	case IMAGE_FILTER_LINEAR:
		return GL_LINEAR;
	}
}

static GLenum get_gl_image_format(image_format_t format)
{
	switch (format)
	{
	default:
	case IMAGE_FORMAT_RGB8:
		return GL_RGB;
	case IMAGE_FORMAT_RGBA8:
		return GL_RGBA;
	}
}

static GLenum get_gl_data_type(image_format_t format)
{
	switch (format)
	{
	default:
	case IMAGE_FORMAT_RGB8:
	case IMAGE_FORMAT_RGBA8:
		return GL_UNSIGNED_BYTE;
	}
}

void image_init(const image_desc_t *desc, image_t **image)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(image != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "An image description is required");
	HE_ASSERT(IMAGE_TYPE_NONE < desc->type < IMAGE_TYPE_COUNT__, "Invalid image type");

	image_t *result = calloc(1, sizeof(image_t));

	glCreateTextures(get_gl_image_type(desc->type), 1, &result->id);
	result->type = desc->type;

	image_t *last_img = image_bind(0, result);

	glTexParameteri(get_gl_image_type(desc->type), GL_TEXTURE_WRAP_S, get_gl_image_wrap(desc->wrap_s));
	glTexParameteri(get_gl_image_type(desc->type), GL_TEXTURE_WRAP_S, get_gl_image_wrap(desc->wrap_t));
	glTexParameteri(get_gl_image_type(desc->type), GL_TEXTURE_MIN_FILTER, get_gl_image_filter(desc->min));
	glTexParameteri(get_gl_image_type(desc->type), GL_TEXTURE_MAG_FILTER, get_gl_image_filter(desc->mag));

	glTexImage2D(get_gl_image_type(desc->type),
		0,
		get_gl_image_format(desc->format),
		desc->size.w,
		desc->size.h,
		0,
		get_gl_image_format(desc->format),
		get_gl_data_type(desc->format),
		desc->data);

	glGenerateMipmap(get_gl_image_type(desc->type));

	image_bind(0, last_img);

	*image = result;
}

image_t *image_create(const image_desc_t *desc)
{
	image_t *image;
	image_init(desc, &image);
	return image;
}

void image_free(image_t *image)
{
	free(image);
}

image_t *image_bind(uint8_t location, image_t *image)
{
	context_t *ctx = context_get_bound();
	HE_ASSERT(ctx != NULL, "A bound context is required");
	HE_ASSERT(0 >= location < IMAGE_MAX_BINDINGS__, "Cannot bind image outside of limits");

	image_t *last_img = ctx->cur_images[location];
	ctx->cur_images[location] = image;

	glActiveTexture(GL_TEXTURE0 + location);
	if (image != NULL)
	{
		glBindTexture(get_gl_image_type(image->type), image->id);
	}
	else
	{
		glBindTexture(get_gl_image_type(last_img->type), 0);
	}

	return last_img;
}
