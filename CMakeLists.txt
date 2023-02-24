cmake_minimum_required(VERSION 3.10)

project(replicant)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB sources "./src/*.cpp")

add_executable(test main.cpp ${sources})
target_include_directories(test PRIVATE ./include/)
