cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC OFF)

add_library(simplecrypt)

target_include_directories(simplecrypt PUBLIC .)

target_sources(simplecrypt PRIVATE
    simplecrypt.cpp
    simplecrypt.h
    )

find_package(Qt5 CONFIG REQUIRED Core)
target_link_libraries(simplecrypt PRIVATE
    Qt5::Core
    3rdparty::openmp
    )

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::simplecrypt ALIAS simplecrypt)
endif()
