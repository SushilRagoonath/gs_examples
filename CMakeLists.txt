cmake_minimum_required(VERSION 3.20)
project(gs_examples C)

set(CMAKE_C_STANDARD 11)
include_directories(third_party/include)
include_directories(microui/external)
add_executable(gs_examples microui/source/main.c microui/source/gs_microui.h)
