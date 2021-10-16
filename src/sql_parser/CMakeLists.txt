add_executable(${PROJECT_NAME} main.cpp)
set_compile_options(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} PRIVATE CLI11 librdb)