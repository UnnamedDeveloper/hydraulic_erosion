#include "imgui_context.h"

#include <stdlib.h>

#include <cimgui_impl.h>

#include "debug/assert.h"

static bool any_context_drawing = false;

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

	// setup backends
	ImGui_ImplGlfw_InitForOpenGL(desc->window->glfw_window, true);
	ImGui_ImplOpenGL3_Init("#version 410 core");

	// set dark theme supreme
	igStyleColorsDark(NULL);

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

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	igDestroyContext(ctx->ctx);
	free(ctx);
}

void imgui_context_begin(imgui_context_t *ctx, float delta)
{
	HE_ASSERT(ctx != NULL, "Cannot begin drawing imgui on NULL");
	HE_ASSERT(!any_context_drawing, "Cannot begin imgui drawing while another context is active drawing");

	any_context_drawing = true;
	ctx->drawing = true;

	igSetCurrentContext(ctx->ctx);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	igNewFrame();
}

void imgui_context_end(imgui_context_t *ctx)
{
	HE_ASSERT(ctx != NULL, "Cannot end drawing imgui on NULL");

	ctx->drawing = false;
	any_context_drawing = false;
}

void imgui_context_render(imgui_context_t *ctx)
{
	HE_ASSERT(ctx != NULL, "Cannot render imgui from NULL");
	HE_ASSERT(!ctx->drawing, "Cannot draw context while drawing");
	HE_ASSERT(!any_context_drawing, "Cannot render imgui while another context is active drawing");

	igSetCurrentContext(ctx->ctx);
	igRender();
	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
}
