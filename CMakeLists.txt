cmake_minimum_required(VERSION 3.14)
project(inter-process-communication)

include_directories(include)
add_library(descriptor src/descriptor.c)

add_subdirectory(ping_pong)
add_subdirectory(guess_number)
