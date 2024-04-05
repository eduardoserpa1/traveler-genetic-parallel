cmake_minimum_required(VERSION 3.27)
project(traveler_genetic_parallel)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -Wall)

include_directories(.)

add_executable(traveler_genetic_parallel
        ./src/City.cpp
        ./src/City.h
        ./src/Main.cpp
        ./src/Route.cpp
        ./src/Route.h)
