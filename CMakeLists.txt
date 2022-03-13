cmake_minimum_required(VERSION 3.21)
project(sysopy C)

set(CMAKE_C_STANDARD 11)

include_directories(lab1)

add_executable(sysopy
        lab1/zad1.c
        lab1/zad1.h
        lab1/zad2.c)
