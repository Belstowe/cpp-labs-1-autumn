cmake_minimum_required(VERSION 3.9)
project(BelMoProTech)

file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES
	${CMAKE_SOURCE_DIR}/src/*.hpp)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

include_directories(
	"${CMAKE_SOURCE_DIR}/src"
	)
