cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(hydraulic_erosion VERSION 0.1.0 LANGUAGES C)

add_executable(${PROJECT_NAME}
	"src/main.c")

set_target_properties(${PROJECT_NAME} PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED TRUE
	C_EXTENSIONS OFF
	FOLDER "${PROJECT_NAME}")

target_include_directories(${PROJECT_NAME} PRIVATE "src")
