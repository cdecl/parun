# morden cmake
cmake_minimum_required(VERSION 3.5)

project(parun)

file(GLOB src "src/*.cpp")
add_executable(parun ${src})

target_include_directories(parun PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(parun PUBLIC -Wall -std=c++17 -O2)
target_link_libraries(parun PUBLIC -lpthread)
