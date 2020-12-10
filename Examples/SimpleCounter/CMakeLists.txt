# Simple Counter Example
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(PROJECT "SimpleCounter")
project(${PROJECT} LANGUAGES CXX)

add_subdirectory("../../" ${CMAKE_CURRENT_BINARY_DIR}/SpinTimer)

add_executable(${PROJECT} "main.cpp")
target_link_libraries(${PROJECT} SpinTimer)