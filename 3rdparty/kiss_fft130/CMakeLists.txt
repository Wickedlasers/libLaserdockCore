cmake_minimum_required(VERSION 3.11)

add_library(kiss_fft130)

set_property(TARGET kiss_fft130 PROPERTY AUTOMOC OFF)

target_include_directories(kiss_fft130 PUBLIC .)

target_sources(kiss_fft130 PRIVATE
    _kiss_fft_guts.h
    kiss_fft.c
    kiss_fft.h
    kissfft.hh
    tools/kiss_fftr.c
    tools/kiss_fftr.h
    )

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(kiss_fft130 PRIVATE -fPIC)
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::kiss_fft130 ALIAS kiss_fft130)
endif()
