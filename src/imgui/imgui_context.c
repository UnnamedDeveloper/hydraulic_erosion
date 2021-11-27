#include "imgui_context.h"

#include <stdlib.h>

#include <cimgui_impl.h>

#include "debug/assert.h"
#include "events/key_event.h"
#include "events/mouse_event.h"
#include "gfx/renderer.h"

static bool any_context_drawing = false;

static void on_mouse_press(event_bus_t *bus, void *user_pointer, mouse_press_event_t *event)
{
	ImGuiIO *io = igGetIO();
	io->MouseDown[event->button] = true;
}

static void on_mouse_release(event_bus_t *bus, void *user_pointer, mouse_release_event_t *event)
{
	ImGuiIO *io = igGetIO();
	io->MouseDown[event->button] = false;
}

static void on_mouse_scroll(event_bus_t *bus, void *user_pointer, mouse_scroll_event_t *event)
{
	ImGuiIO *io = igGetIO();
	io->MouseWheelH += (float)event->offset[0];
	io->MouseWheel += (float)event->offset[1];
}

static void on_char_typed(event_bus_t *bus, void *user_pointer, char_type_event_t *event)
{
	ImGuiIO *io = igGetIO();
	ImGuiIO_AddInputCharacter(io, event->c);
}

static void on_key_press(event_bus_t *bus, void *user_pointer, key_press_event_t *event)
{
	ImGuiIO *io = igGetIO();
	io->KeysDown[event->key] = true;
}

static void on_key_release(event_bus_t *bus, void *user_pointer, key_press_event_t *event)
{
	ImGuiIO *io = igGetIO();
	io->KeysDown[event->key] = false;
}

void imgui_context_init(const imgui_context_desc_t *desc, imgui_context_t **ctx)
{
	HE_ASSERT(ctx != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "An imgui contexxt description is required");
	HE_ASSERT(desc->window != NULL, "A window is required");

	imgui_context_t *result = malloc(sizeof(imgui_context_t));

	result->ctx = igCreateContext(NULL);
	result->io = igGetIO();
	result->window = desc->window;
	result->event_bus = desc->event_bus;
	result->drawing = false;

	// setup backends
	ImGui_ImplGlfw_InitForOpenGL(desc->window->glfw_window, false);
	ImGui_ImplOpenGL3_Init("#version 410 core");

	// setup event callbacks
	result->mpress_cb_id   = event_subscribe(desc->event_bus, EVENT_TYPE_MOUSE_PRESS,   NULL, (event_callback_fn_t)on_mouse_press);
	result->mrelease_cb_id = event_subscribe(desc->event_bus, EVENT_TYPE_MOUSE_RELEASE, NULL, (event_callback_fn_t)on_mouse_release);
	result->mscroll_cb_id  = event_subscribe(desc->event_bus, EVENT_TYPE_MOUSE_SCROLL,  NULL, (event_callback_fn_t)on_mouse_scroll);
	result->char_cb_id     = event_subscribe(desc->event_bus, EVENT_TYPE_CHAR_TYPE,     NULL, (event_callback_fn_t)on_char_typed);
	result->kpress_cb_id   = event_subscribe(desc->event_bus, EVENT_TYPE_KEY_PRESS,     NULL, (event_callback_fn_t)on_key_press);
	result->krelease_cb_id = event_subscribe(desc->event_bus, EVENT_TYPE_KEY_RELEASE,   NULL, (event_callback_fn_t)on_key_release);

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

	event_unsubscribe(ctx->event_bus, EVENT_TYPE_MOUSE_PRESS,   ctx->mpress_cb_id);
	event_unsubscribe(ctx->event_bus, EVENT_TYPE_MOUSE_RELEASE, ctx->mrelease_cb_id);
	event_unsubscribe(ctx->event_bus, EVENT_TYPE_MOUSE_SCROLL,  ctx->mscroll_cb_id);
	event_unsubscribe(ctx->event_bus, EVENT_TYPE_CHAR_TYPE,     ctx->char_cb_id);
	event_unsubscribe(ctx->event_bus, EVENT_TYPE_KEY_PRESS,     ctx->kpress_cb_id);
	event_unsubscribe(ctx->event_bus, EVENT_TYPE_KEY_RELEASE,   ctx->krelease_cb_id);

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

	context_t *last_ctx = context_bind(ctx->window->context);

	ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());

	context_bind(last_ctx);
}
