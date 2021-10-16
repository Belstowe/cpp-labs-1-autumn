file(GLOB_RECURSE TEST_SOURCE_FILES 
	main.cpp
	librdb/*.cpp)
set(SOURCES ${TEST_SOURCE_FILES})

add_executable(${PROJECT_NAME}_test main.cpp librdb/sum.test.cpp)
target_link_libraries(${PROJECT_NAME}_test PRIVATE sum_lib gtest gtest_main)
target_compile_features(${PROJECT_NAME}_test PUBLIC cxx_std_11)
