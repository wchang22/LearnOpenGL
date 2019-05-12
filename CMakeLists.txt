cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

set(CMAKE_CXX_STANDARD 17)

project(learnopengl)

include_directories(include)
link_directories(include)

find_package(assimp REQUIRED)
find_package(OpenMP REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES src/* shaders/* include/*)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lglfw -lGL -ldl -fopenmp -lassimp")

add_executable(${PROJECT_NAME} ${SOURCES})
