# sources
set(OPENLASE_SOURCE_FILES
    libol.c
    libol.h
    )

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# library
add_library(OpenLase SHARED ${OPENLASE_SOURCE_FILES})
target_link_libraries(OpenLase Qt5::Core)
