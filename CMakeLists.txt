cmake_minimum_required(VERSION 3.24)
project(Sandpile_Model LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(lib)

add_executable(sandpile main.cpp)

target_link_libraries(sandpile PUBLIC Sandpile)
