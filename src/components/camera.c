#include "camera.h"

#include <math.h>

#include <GLFW/glfw3.h>

#include "debug/assert.h"
#include "events/mouse_event.h"
#include "events/window_event.h"

static void on_window_resize(event_bus_t *bus, void *user_pointer, window_resize_event_t *event)
{
	camera_t *camera = (camera_t *)user_pointer;
	camera_update_projection(camera);
}

static void on_mouse_move(event_bus_t *bus, void *user_pointer, mouse_move_event_t *event)
{
	camera_t *camera = (camera_t *)user_pointer;
	if (camera->move)
		camera_move(camera, 0, (vec2){ -event->offset[0] / 5.0f, event->offset[1] / 5.0f });
}

static void on_mouse_press(event_bus_t *bus, void *user_pointer, mouse_press_event_t *event)
{
	camera_t *camera = (camera_t *)user_pointer;
	if (event->button == GLFW_MOUSE_BUTTON_LEFT)
		camera->move = true;
}

static void on_mouse_release(event_bus_t *bus, void *user_pointer, mouse_release_event_t *event)
{
	camera_t *camera = (camera_t *)user_pointer;
	if (event->button == GLFW_MOUSE_BUTTON_LEFT)
		camera->move = false;
}

static void on_mouse_scroll(event_bus_t *bus, void *user_pointer, mouse_scroll_event_t *event)
{
	camera_t *camera = (camera_t *)user_pointer;
	camera_move(camera, event->offset[1], GLM_VEC2_ZERO);
}

static void update_camera_position(camera_t *camera)
{
	float horizontal_dist = camera->distance * cos(glm_rad(camera->angle[1]));
	float vertical_dist   = camera->distance * sin(glm_rad(camera->angle[1]));

	float theta = glm_rad(camera->angle[0]);
	camera->position[0] = camera->target[0] - (horizontal_dist * sin(theta));
	camera->position[2] = camera->target[2] - (horizontal_dist * cos(theta));
	camera->position[1] = camera->target[1] - vertical_dist;
}

void camera_init(const camera_desc_t *desc, camera_t **camera)
{
	HE_ASSERT(camera != NULL, "Cannot initialize NULL");
	HE_ASSERT(desc != NULL, "A camera description is required");

	camera_t *result = calloc(1, sizeof(camera_t));

	result->window = desc->window;

	glm_vec3_zero(result->target);
	glm_vec3_zero(result->position);
	glm_vec2_zero(result->angle);
	glm_vec2_copy(desc->angle, result->angle);
	result->fov = desc->fov;
	result->distance = desc->distance;
	glm_vec3_copy(desc->target, result->target);

	camera_update_projection(result);
	update_camera_position(result);

	event_subscribe(desc->window->event_bus, EVENT_TYPE_WINDOW_RESIZE, result, (event_callback_fn_t)on_window_resize);
	event_subscribe(desc->window->event_bus, EVENT_TYPE_MOUSE_MOVE,    result, (event_callback_fn_t)on_mouse_move);
	event_subscribe(desc->window->event_bus, EVENT_TYPE_MOUSE_SCROLL,  result, (event_callback_fn_t)on_mouse_scroll);
	event_subscribe(desc->window->event_bus, EVENT_TYPE_MOUSE_PRESS,   result, (event_callback_fn_t)on_mouse_press);
	event_subscribe(desc->window->event_bus, EVENT_TYPE_MOUSE_RELEASE, result, (event_callback_fn_t)on_mouse_release);

	*camera = result;
}

camera_t *camera_create(const camera_desc_t *desc)
{
	camera_t *camera;
	camera_init(desc, &camera);
	return camera;
}

void camera_free(camera_t *camera)
{
	free(camera);
}

void camera_update_projection(camera_t *camera)
{
	uvec2 size = window_get_size(camera->window);
	glm_perspective(glm_rad(camera->fov), (float)size.x / (float)size.y, 0.1f, 1000.0f, camera->projection);
}

void camera_create_view_matrix(camera_t *camera, mat4 view)
{
	glm_mat4_identity(view);
	glm_lookat(camera->position, camera->target, (vec3){ 0.0f, 1.0f, 0.0f }, view);
}

void camera_move(camera_t *camera, float distance_offset, vec2 angle_offset)
{
	camera->distance += distance_offset;
	glm_vec2_add(camera->angle, angle_offset, camera->angle);

	update_camera_position(camera);
}

void camera_set_target(camera_t *camera, vec3 target)
{
	glm_vec3_copy(target, camera->target);
	update_camera_position(camera);
}
