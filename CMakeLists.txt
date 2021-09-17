cmake_minimum_required(VERSION 3.11..3.21)
project(
	BelMoProTech
	VERSION 0.1
	LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(CTest)

add_subdirectory(src)
