#include "imgui_context.h"

#include <stdlib.h>

#include "debug/assert.h"

void imgui_context_init(const imgui_context_desc_t *desc, imgui_context_t **ctx)
{
	HE_ASSERT(ctx != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "An imgui contexxt description is required");
	HE_ASSERT(desc->window != NULL, "A window is required");

	imgui_context_t *result = malloc(sizeof(imgui_context_t));

	result->ctx = igCreateContext(NULL);
	result->io = igGetIO();
	result->window = desc->window;
	result->drawing = false;

	// generate font atlas
	unsigned char *text_pixels = NULL;
	int text_w, text_h;
	ImFontAtlas_GetTexDataAsRGBA32(result->io->Fonts, &text_pixels, &text_w, &text_h, NULL);

	*ctx = result;
}

imgui_context_t *imgui_context_create(const imgui_context_desc_t *desc)
{
	imgui_context_t *ctx;
	imgui_context_init(desc, &ctx);
	return ctx;
}

void imgui_context_free(imgui_context_t *ctx)
{
	if (ctx == NULL) return;

	igDestroyContext(ctx->ctx);
	free(ctx);
}

void imgui_context_begin(imgui_context_t *ctx, float delta)
{
	HE_ASSERT(ctx != NULL, "Cannot begin drawing imgui on NULL");

	ctx->drawing = true;

	uvec2 usize = window_get_size(ctx->window);
	ctx->io->DisplaySize = (ImVec2){ .x = (int)usize.x, .y = (int)usize.y };
	ctx->io->DeltaTime = delta;
	igNewFrame();
}

void imgui_context_end(imgui_context_t *ctx)
{
	HE_ASSERT(ctx != NULL, "Cannot end drawing imgui on NULL");
	ctx->drawing = false;
}

void imgui_context_render(imgui_context_t *ctx)
{
	HE_ASSERT(ctx != NULL, "Cannot render imgui from NULL");

	igRender();
}
