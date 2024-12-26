cmake_minimum_required(VERSION 3.10)
project(Chip-8)

set(CMAKE_CXX_STANDARD 24)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

set(SOURCES
    src/main.c
)

add_executable(c8 ${SOURCES})
