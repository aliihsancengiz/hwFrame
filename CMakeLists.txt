cmake_minimum_required(VERSION 3.10)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

project(FrameDecoder)

add_executable(execMain main.cpp)

set(CMAKE_CXX_FLAGS "-O2 -fno-exceptions  -pedantic -lpthread -Wall")
target_include_directories(execMain PUBLIC sml/include/boost)
