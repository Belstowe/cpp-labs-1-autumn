cmake_minimum_required(VERSION 3.11..3.21)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(${CMAKE_MODULE_PATH}/CompileOptions.cmake)
include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
	include(${CMAKE_MODULE_PATH}/add_FetchContent_MakeAvailable.cmake)
endif()

project(
	SQLParser
	VERSION 0.1
	LANGUAGES CXX)

find_program(CLANG_TIDY_EXE NAMES "clang-tidy" "clang-tidy-*")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(extras)
