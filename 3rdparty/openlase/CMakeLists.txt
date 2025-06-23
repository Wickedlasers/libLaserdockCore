cmake_minimum_required(VERSION 3.11)

# sources
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# library
if(IOS)
    add_library(openlase)
else()
    add_library(openlase SHARED)
endif()

target_sources(openlase PRIVATE
    src/ldLibol.cpp
    include/openlase/ldLibol.h)

find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED Core)
target_link_libraries(openlase PRIVATE Qt::Core)

target_include_directories(openlase PUBLIC include)

target_include_directories(openlase PRIVATE src)

if(WIN32)
    target_compile_definitions(openlase PRIVATE _USE_MATH_DEFINES)
    target_compile_options(openlase PRIVATE /wd4244) # conversion from double to int
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::openlase ALIAS openlase)
endif()
