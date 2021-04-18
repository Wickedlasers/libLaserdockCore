cmake_minimum_required(VERSION 3.11)

add_library(kiss_fft130)

set_property(TARGET kiss_fft130 PROPERTY AUTOMOC OFF)
set_property(TARGET kiss_fft130 PROPERTY AUTORCC OFF)
set_property(TARGET kiss_fft130 PROPERTY AUTOUIC OFF)

target_include_directories(kiss_fft130 PUBLIC .)

target_sources(kiss_fft130 PRIVATE
    _kiss_fft_guts.h
    kiss_fft.c
    kiss_fft.h
    kissfft.hh
    tools/kiss_fftr.c
    tools/kiss_fftr.h
    )

# TODO: Check if OpenMP has sense
# Note by Sergey: from my check on mac CPU is 100% for all cores if _OPENMP is enabled
# Most probably we don't need it here
#if(LD_CORE_ENABLE_OPENMP)
#    target_compile_definitions(kiss_fft130 PRIVATE _OPENMP)
#    target_link_libraries(kiss_fft130 PUBLIC
#        3rdparty::openmp
#        )
#endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(kiss_fft130 PRIVATE -fPIC)
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::kiss_fft130 ALIAS kiss_fft130)
endif()
