file(GLOB_RECURSE TEST_SOURCE_FILES 
	main.cpp
	libcalc/*.cpp)
set(SOURCES ${TEST_SOURCE_FILES})

add_executable(calc_test main.cpp libcalc/sum.test.cpp)
target_link_libraries(calc_test PRIVATE sum_lib gtest gtest_main)
target_compile_features(calc_test PUBLIC cxx_std_11)
