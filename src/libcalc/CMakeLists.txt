add_library(sum_lib sum.cpp sum.hpp)
set_compile_options(sum_lib)

target_include_directories(sum_lib PUBLIC ./)
