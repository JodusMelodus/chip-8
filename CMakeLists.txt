cmake_minimum_required(VERSION 3.16)
project(project_name)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.c" "src/*.cpp")
add_executable(main ${SOURCES})

include_directories(include)