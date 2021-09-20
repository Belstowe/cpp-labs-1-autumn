cmake_minimum_required(VERSION 3.11..3.21)

project(
	BelMoProTech
	VERSION 0.1
	LANGUAGES CXX)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" "clang-tidy-*")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(${CMAKE_MODULE_PATH}/CompileOptions.cmake)
include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
	include(${CMAKE_MODULE_PATH}/add_FetchContent_MakeAvailable.cmake)
endif()

add_executable(calc_app ./src/calc/main.cpp)
target_link_libraries(calc_app PRIVATE sum_lib CLI11)
set_compile_options(calc_app)

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(extras)
