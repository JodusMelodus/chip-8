cmake_minimum_required(VERSION 3.16)
project(chip-8)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.c")
add_executable(main ${SOURCES})

include_directories(include)