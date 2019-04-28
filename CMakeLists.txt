cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

set(CMAKE_CXX_STANDARD 17)

project(learnopengl)

include_directories(include)
link_directories(include)

file(GLOB_RECURSE SOURCES src/*.cpp src/*.h shaders/*glsl include/*)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl")

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
