cmake_minimum_required(VERSION 3.20)
project(Grace VERSION 0.1.0 LANGUAGES CXX)

# We require C++23 for the "Routing Theorem" (Concepts & Consteval)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find the Engine's fuel: liburing
find_library(URING_LIB uring REQUIRED)

# 2. Build the Compiled Core (The Engine Room)
add_library(grace_core STATIC src/engine.cpp)
target_link_libraries(grace_core PRIVATE ${URING_LIB})

# 3. Define the Header-Only API (The Bridge)
add_library(grace_api INTERFACE)
target_include_directories(grace_api INTERFACE include/)

# 4. Create an executable for our Hello World example
add_executable(hello_world examples/hello_world.cpp)
target_link_libraries(hello_world PRIVATE grace_core grace_api)

message(STATUS "⚓ Grace Framework: Ready for Construction")
