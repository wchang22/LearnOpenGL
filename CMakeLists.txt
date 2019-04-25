cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(learnopengl)

include_directories(include)
link_directories(include)

set(SOURCES main.cpp glad.c)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl")

add_executable(${PROJECT_NAME} ${SOURCES})
