file(GLOB_RECURSE LIBRDB_SOURCE_FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB_RECURSE LIBRDB_HEADER_FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(librdb ${LIBRDB_SOURCE_FILES} ${LIBRDB_HEADER_FILES})
set_compile_options(librdb)
target_include_directories(librdb PRIVATE ./)