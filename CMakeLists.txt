# minimum version of cmake 3.10
cmake_minimum_required(VERSION 3.13)

project(rpc-cli VERSION 0.1)

# c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# executable
add_executable(rpc-cli)

# source files
target_sources(rpc-cli
  PUBLIC main.cpp
  PUBLIC program_options.cpp)
