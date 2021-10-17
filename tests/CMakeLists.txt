set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)

file(GLOB_RECURSE TEST_SOURCE_FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(SOURCES ${TEST_SOURCE_FILES})

add_executable(${PROJECT_NAME}_test ${TEST_SOURCE_FILES})
set_compile_options(${PROJECT_NAME}_test)
target_link_libraries(${PROJECT_NAME}_test PRIVATE librdb gtest gtest_main)
target_compile_features(${PROJECT_NAME}_test PUBLIC cxx_std_11)
