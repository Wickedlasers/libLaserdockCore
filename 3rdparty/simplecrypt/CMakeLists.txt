cmake_minimum_required(VERSION 3.0)

add_library(simplecrypt)

target_sources(simplecrypt PRIVATE
    simplecrypt.cpp
    simplecrypt.h
    )

target_link_libraries(simplecrypt PRIVATE Qt5::Core)
