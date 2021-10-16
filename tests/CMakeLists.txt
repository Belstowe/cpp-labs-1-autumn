file(GLOB_RECURSE TEST_SOURCE_FILES 
		${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(SOURCES ${TEST_SOURCE_FILES})

add_executable(${PROJECT_NAME}_test ${TEST_SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}_test PRIVATE gtest gtest_main)
target_compile_features(${PROJECT_NAME}_test PUBLIC cxx_std_11)
