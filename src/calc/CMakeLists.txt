add_executable(calc_app main.cpp)
set_compile_options(calc_app)

target_link_libraries(calc_app PRIVATE sum_lib CLI11)