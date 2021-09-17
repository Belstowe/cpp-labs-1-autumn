add_executable(calc_app main.cpp)

target_link_libraries(calc_app PRIVATE sum_lib)
target_compile_features(calc_app PUBLIC cxx_std_11)
