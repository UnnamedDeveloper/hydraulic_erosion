#ifndef __imgui_imgui_context__
#define __imgui_imgui_context__

#include <stdbool.h>

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>

#include "events/event.h"
#include "gfx/window.h"

typedef struct imgui_context_desc_t
{
	window_t *window;
	event_bus_t *event_bus;
} imgui_context_desc_t;

typedef struct imgui_context_t
{
	struct ImGuiContext *ctx;
	struct ImGuiIO *io;

	window_t *window;
	event_bus_t *event_bus;

	uint32_t mpress_cb_id;
	uint32_t mrelease_cb_id;
	uint32_t mscroll_cb_id;
	uint32_t char_cb_id;
	uint32_t kpress_cb_id;
	uint32_t krelease_cb_id;

	bool drawing;
} imgui_context_t;

void imgui_context_init(const imgui_context_desc_t *desc, imgui_context_t **ctx);
imgui_context_t *imgui_context_create(const imgui_context_desc_t *desc);
void imgui_context_free(imgui_context_t *ctx);

void imgui_context_begin(imgui_context_t *ctx, float delta);
void imgui_context_end(imgui_context_t *ctx);
void imgui_context_render(imgui_context_t *ctx);

#endif /* __imgui_imgui_context__ */
