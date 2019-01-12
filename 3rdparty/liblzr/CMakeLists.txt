cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# library
add_library(liblzr SHARED)

target_sources(liblzr PRIVATE
    #    engine.hpp
        liblzr.hpp
    #    src/engine_serialization.cpp
    #    src/engine.cpp
        src/frame.cpp
        src/ilda.cpp
        src/ilda.hpp
        src/ilda_palette.cpp
        src/ilda_read.cpp
        src/ilda_write.cpp
        src/interpolator.cpp
        src/interpolator.hpp
        src/mask.cpp
        src/optimizer.cpp
        src/optimizer.hpp
        src/optimizer_compile_paths.cpp
        src/optimizer_find_paths.cpp
        src/optimizer_path.cpp
        src/optimizer_reorder_paths.cpp
        src/point.cpp
        src/reduce.cpp
        src/transforms.cpp
        src/include/portable_endian.h
    )

# win library
if(WIN32)
    find_library (WS2_32_LIB NAMES Ws2_32)
    target_link_libraries(liblzr ${WS2_32_LIB})
endif()

if(ANDROID)
    target_compile_definitions(liblzr PRIVATE __STDC_LIMIT_MACROS)
endif()

