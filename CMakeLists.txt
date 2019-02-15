cmake_minimum_required(VERSION 3.5)

project(parun)

#add_subdirectory(/subdir)
set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O2")
include_directories(clipp/include ThreadPool)
# link_directories(/usr/local/opt/openssl/lib)
link_libraries(-pthread )

# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

file(GLOB src "src/*.cpp")
#set(src xxx.cpp)

add_executable(parun ${src})

# add_library(libname SHARED ${src})
# add_library(libname-static STATIC ${src})
# set_target_properties(libname-static PROPERTIES OUTPUT_NAME libname)

