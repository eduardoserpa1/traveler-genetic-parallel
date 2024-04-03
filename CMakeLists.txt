cmake_minimum_required(VERSION 3.27)
project(traveler_genetic_parallel)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(traveler_genetic_parallel
        City.cpp
        City.h
        Main.cpp
        Route.cpp
        Route.h)
