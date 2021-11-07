cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(hydraulic_erosion VERSION 0.1.0 LANGUAGES C)

add_subdirectory("deps/cglm")
add_subdirectory("deps/glad")
add_subdirectory("deps/glfw")

add_executable(${PROJECT_NAME}
	"src/main.c"
	"src/app.h" "src/app.c"
	"src/debug/assert.h" "src/debug/assert.c"
	"src/events/event.h" "src/events/event.c" "src/events/window_event.h" "src/events/window_event.c"
	"src/gfx/buffer.h" "src/gfx/buffer.c" "src/gfx/context.h" "src/gfx/context.c" "src/gfx/image.h" "src/gfx/image.c" "src/gfx/pipeline.h" "src/gfx/pipeline.c" "src/gfx/renderer.h" "src/gfx/renderer.c" "src/gfx/window.h" "src/gfx/window.c"
	"src/math/types.h" "src/math/types.c")

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED TRUE
	C_EXTENSIONS OFF
	FOLDER "${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME} PRIVATE "src")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/stb/")

target_link_libraries(${PROJECT_NAME} PRIVATE cglm)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

set(PROJECT_RESOURCES)

foreach(RESOURCE IN LISTS PROJECT_RESOURCES)
	configure_file("${RESOURCE}" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE}" COPYONLY)
endforeach()
