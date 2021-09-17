cmake_minimum_required(VERSION 3.11..3.21)
project(
	BelMoProTech
	VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(CTest)
include(FetchContent)
if(${CMAKE_VERSION} VERSION_LESS 3.14)
	include(cmake/add_FetchContent_MakeAvailable.cmake)
endif()

add_executable(calc_app ./src/calc/main.cpp)
target_link_libraries(calc_app PRIVATE sum_lib CLI11)
target_compile_features(calc_app PUBLIC cxx_std_11)

add_subdirectory(src)
add_subdirectory(extras)
