cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(hydraulic_erosion LANGUAGES C CXX)

add_subdirectory("deps/cglm")
add_subdirectory("deps/glad")
add_subdirectory("deps/glfw")

# setup cimgui target
add_library(cimgui STATIC 
    "deps/cimgui/cimgui.cpp"
    "deps/cimgui/cimgui.h"
    "deps/cimgui/imgui/imgui.cpp"
    "deps/cimgui/imgui/imgui.h"
    "deps/cimgui/imgui/imgui_widgets.cpp"
    "deps/cimgui/imgui/imgui_draw.cpp"
    "deps/cimgui/imgui/imgui_tables.cpp"
    "deps/cimgui/imgui/imgui_demo.cpp"
    "deps/cimgui/imgui/backends/imgui_impl_glfw.cpp"
    "deps/cimgui/imgui/backends/imgui_impl_glfw.h"
    "deps/cimgui/imgui/backends/imgui_impl_opengl3.cpp"
    "deps/cimgui/imgui/backends/imgui_impl_opengl3.h"
    )
target_include_directories(cimgui PUBLIC "deps/cimgui" "deps/cimgui/imgui" "deps/cimgui/generator/output")
add_definitions("-DIMGUI_IMPL_API=extern \"C\" ")
target_link_libraries(cimgui PRIVATE glfw)
set_target_properties(cimgui PROPERTIES LINKER_LANGUAGE CXX)

option(SHOW_CONSOLE "If the program should be compiled as a console application" OFF)

set(PROJECT_SOURCES
	"src/main.c"
	"src/app.h" "src/app.c" "src/erosion.h" "src/erosion.c"
	"src/components/camera.h" "src/components/camera.c" "src/components/terrain.h" "src/components/terrain.c"
	"src/debug/assert.h" "src/debug/assert.c"
	"src/events/event.h" "src/events/event.c" "src/events/key_event.h" "src/events/key_event.c" "src/events/mouse_event.h" "src/events/mouse_event.c" "src/events/window_event.h" "src/events/window_event.c"
	"src/gfx/buffer.h" "src/gfx/buffer.c" "src/gfx/context.h" "src/gfx/context.c" "src/gfx/image.h" "src/gfx/image.c" "src/gfx/mesh.h" "src/gfx/mesh.c" "src/gfx/pipeline.h" "src/gfx/pipeline.c" "src/gfx/renderer.h" "src/gfx/renderer.c" "src/gfx/window.h" "src/gfx/window.c"
	"src/imgui/imgui_context.c" "src/imgui/imgui_context.h"
	"src/io/file.h" "src/io/file.c"
	"src/math/types.h" "src/math/types.c" "src/math/noise.h" "src/math/noise.c")

if (${SHOW_CONSOLE})
	add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})
else()
	add_executable(${PROJECT_NAME} WIN32 ${PROJECT_SOURCES})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED TRUE
	C_EXTENSIONS OFF
	LINKER_LANGUAGE C
	FOLDER "${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME} PRIVATE "src")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb/")

target_link_libraries(${PROJECT_NAME} PRIVATE cglm)
target_link_libraries(${PROJECT_NAME} PRIVATE cimgui)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
if (NOT MSVC)
target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

set(PROJECT_RESOURCES
	"res/shaders/terrain.vs.glsl" "res/shaders/terrain.fs.glsl" "res/shaders/terrain_wireframe.fs.glsl"
	"res/shaders/imgui.vs.glsl" "res/shaders/imgui.fs.glsl")

foreach(RESOURCE IN LISTS PROJECT_RESOURCES)
	configure_file("${RESOURCE}" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE}" COPYONLY)
endforeach()
