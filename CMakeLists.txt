cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(hydraulic_erosion LANGUAGES C)

add_subdirectory("deps/cglm")
add_subdirectory("deps/glad")
add_subdirectory("deps/glfw")

add_executable(${PROJECT_NAME}
	"src/main.c"
	"src/app.h" "src/app.c"
	"src/components/camera.h" "src/components/camera.c" "src/components/terrain.h" "src/components/terrain.c"
	"src/debug/assert.h" "src/debug/assert.c"
	"src/events/event.h" "src/events/event.c" "src/events/key_event.h" "src/events/key_event.c" "src/events/mouse_event.h" "src/events/mouse_event.c" "src/events/window_event.h" "src/events/window_event.c"
	"src/gfx/buffer.h" "src/gfx/buffer.c" "src/gfx/context.h" "src/gfx/context.c" "src/gfx/image.h" "src/gfx/image.c" "src/gfx/mesh.h" "src/gfx/mesh.c" "src/gfx/pipeline.h" "src/gfx/pipeline.c" "src/gfx/renderer.h" "src/gfx/renderer.c" "src/gfx/window.h" "src/gfx/window.c"
	"src/io/file.h" "src/io/file.c"
	"src/math/types.h" "src/math/types.c")

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED TRUE
	C_EXTENSIONS OFF
	FOLDER "${PROJECT_NAME}")

if (WIN32)
	# the default visual studio stack size failed me
	set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/STACK:10000000")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "src")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb/")

target_link_libraries(${PROJECT_NAME} PRIVATE cglm)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

set(PROJECT_RESOURCES
	"res/shaders/terrain.vs.glsl" "res/shaders/terrain.fs.glsl" "res/shaders/terrain_wireframe.fs.glsl")

foreach(RESOURCE IN LISTS PROJECT_RESOURCES)
	configure_file("${RESOURCE}" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE}" COPYONLY)
endforeach()
