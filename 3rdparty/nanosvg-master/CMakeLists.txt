cmake_minimum_required(VERSION 3.11)

add_library(nanosvg INTERFACE)

# header-only library
target_include_directories(nanosvg INTERFACE src)

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::nanosvg ALIAS nanosvg)
endif()
