cmake_minimum_required(VERSION 3.0)

add_library(simplecrypt)

set_property(TARGET simplecrypt PROPERTY AUTOMOC OFF)
set_property(TARGET simplecrypt PROPERTY AUTORCC OFF)
set_property(TARGET simplecrypt PROPERTY AUTOUIC OFF)

target_include_directories(simplecrypt PUBLIC .)

target_sources(simplecrypt PRIVATE
    simplecrypt.cpp
    simplecrypt.h
    )

find_package(Qt5 CONFIG REQUIRED Core)
target_link_libraries(simplecrypt PRIVATE
    Qt5::Core
    )


if(LD_CORE_ENABLE_OPENMP)
    target_link_libraries(simplecrypt PUBLIC
        3rdparty::openmp
        )
endif()


if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::simplecrypt ALIAS simplecrypt)
endif()
