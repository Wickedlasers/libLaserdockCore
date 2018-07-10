
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ANDROID)
    add_definitions(-D__STDC_LIMIT_MACROS)
endif()

# sources

set(LIBLZR_SOURCE_FILES
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

# library
add_library(liblzr SHARED ${LIBLZR_SOURCE_FILES})

# win library
if(WIN32)
    find_library (WS2_32_LIB NAMES Ws2_32)
    target_link_libraries(liblzr ${WS2_32_LIB})
endif()
