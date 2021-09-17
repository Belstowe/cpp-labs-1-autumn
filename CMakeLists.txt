cmake_minimum_required(VERSION 3.11..3.21)
project(
	BelMoProTech
	VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(CTest)

add_executable(calc_app ./src/calc/main.cpp)
target_link_libraries(calc_app PRIVATE sum_lib)
target_compile_features(calc_app PUBLIC cxx_std_11)

add_subdirectory(src)
