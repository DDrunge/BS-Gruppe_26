cmake_minimum_required(VERSION 3.7.2)
project(TestRM C)

set(CMAKE_C_STANDARD 11)

add_executable(TestRM main.c keyValStore.h sub.h)
