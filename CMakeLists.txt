cmake_minimum_required(VERSION 3.22)
project(ex3)

set(CMAKE_CXX_STANDARD 11)

add_executable(ex3 main.cpp BoundedQueue.h UnboundedQueue.h)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
