cmake_minimum_required(VERSION 3.11)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
endif()

# android build flag
option(LD_ANDROID_BUILD "Build for Android" OFF)
# main app configuration
option(LD_CORE_BUILD_EXAMPLE "Build Example" ON)
option(LD_CORE_BUILD_TESTS "Build Tests" OFF)
# library flags
option(LD_CORE_ENABLE_LIBLZR "Enable liblzr" OFF)
# Android resources extraction
option(LD_CORE_RESOURCES_EXTRACTOR "Support of resources extraction (android)" OFF)

include(CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(LD_CORE_RESOURCES_EXTRACTOR "Enable resource extractor" OFF
    "NOT LD_ANDROID_BUILD" ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12")
endif()

if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
    set(LD_BUILD_RELEASE_VERSION ON)
endif()

# setup C++
set(CMAKE_VERBOSE_MAKEFILE ON)

# setup C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# load local
include(local.cmake OPTIONAL)

include(DetectQt)
include(SetupAndroidToolchain)

project(ldCore)

add_subdirectory(3rdparty)
add_subdirectory(ldCore)

# add example for library
if(LD_CORE_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(LD_CORE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(CMAKE_GENERATOR STREQUAL Xcode)
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()


