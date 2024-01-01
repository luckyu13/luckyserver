cmake_minimum_required(VERSION 3.0.0)

project(luckyserver VERSION 1.0.0)

file(GLOB src_cpp "src/*.cpp")

add_executable(server ${src_cpp})
target_compile_options(server PRIVATE -g -DDEBUG)
target_include_directories(server PRIVATE inc)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
