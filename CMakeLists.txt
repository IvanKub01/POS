cmake_minimum_required(VERSION 3.7)
project(untitled3 C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled3 main.c klient.c klient.h server.c server.h)

target_link_libraries(untitled3 pthread)