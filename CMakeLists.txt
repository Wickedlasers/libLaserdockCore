cmake_minimum_required(VERSION 3.11)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_LIST_DIR)
    message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
endif()

# platform build flag
option(LD_ANDROID_BUILD "Build for Android" OFF)

# @deprecated Qt5 flag, Qt6 don't need it
option(LD_IOS_BUILD "Build for iOS" OFF)
option(LD_ENABLE_QT5COMPAT "Enable Qt5Compat module" ON)

# main app configuration
option(LD_CORE_BUILD_EXAMPLE "Build Example" ON)
option(LD_CORE_BUILD_TESTS "Build Tests" OFF)
# library flags
option(LD_CORE_ENABLE_GAMES "Enable Games" OFF)
option(LD_CORE_ENABLE_BOX2D "Enable Box2D" OFF)
option(LD_CORE_ENABLE_LIBLZR "Enable liblzr" OFF)
option(LD_CORE_ENABLE_LUAGAME "Enable lua game module" OFF)
option(LD_CORE_FORCE_QGAMEPAD "Enable QGamepad backend" OFF)
option(LD_CORE_ENABLE_RAZER "Enable ldRazer game module" OFF)
option(LD_CORE_ENABLE_QUAZIP "Support of resources extraction (android)" OFF)
# Android resources extraction
option(LD_CORE_RESOURCES_EXTRACTOR "Support of resources extraction (android)" OFF)

message(STATUS "LD_CORE_BUILD_EXAMPLE ${LD_CORE_ENABLE_GAMES}")
message(STATUS "LD_CORE_ENABLE_GAMES ${LD_CORE_BUILD_EXAMPLE}")

if(LD_CORE_ENABLE_GAMES)
    message(STATUS "LD_CORE_FORCE_QGAMEPAD ${LD_CORE_FORCE_QGAMEPAD}")
endif()

include(CMakeDependentOption)

CMAKE_DEPENDENT_OPTION(LD_CORE_ENABLE_BOX2D "" OFF
    "NOT LD_CORE_BUILD_EXAMPLE" ON)
CMAKE_DEPENDENT_OPTION(LD_CORE_ENABLE_LUAGAME "" OFF
    "NOT LD_CORE_BUILD_EXAMPLE" ON)
CMAKE_DEPENDENT_OPTION(LD_CORE_ENABLE_RAZER "" OFF
    "NOT LD_CORE_BUILD_EXAMPLE" ON)

message(STATUS "LD_CORE_ENABLE_BOX2D ${LD_CORE_ENABLE_BOX2D}")

if(APPLE AND NOT LD_IOS_BUILD)
    set(CMAKE_OSX_DEPLOYMENT_TARGET ${LD_MACOS_DEPLOYMENT_TARGET})
    # ALLOW LASEROS TO BUILD ON AN APPLE MAC M1 FOR X86_64 PLATFOIRM
    SET(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architectures for OSX")
endif()

if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
    set(LD_BUILD_RELEASE_VERSION ON)
endif()

# setup C++
set(CMAKE_VERBOSE_MAKEFILE ON)

# setup C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# load local
include(local.cmake OPTIONAL)

include(SetupAndroidToolchain)
include(FixZstd)

if(LD_IOS_BUILD)
    SET(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")

    set(ENABLE_BITCODE OFF)
    set(PLATFORM OS64COMBINED)
    if(QT_VERSION_MAJOR EQUAL 6)
        set(DEPLOYMENT_TARGET "14.0")
    else()
        set(DEPLOYMENT_TARGET "12.0")
    endif()

#    set(ARCHS x86_64)
    if(QT_VERSION_MAJOR EQUAL 5)
        set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/ios.toolchain.cmake)
    endif()

    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
endif()


project(ldCore)

include(DetectLinux)
include(DetectWin64)

CMAKE_DEPENDENT_OPTION(LD_CORE_RESOURCES_EXTRACTOR "" OFF
    "NOT ANDROID" ON)

CMAKE_DEPENDENT_OPTION(LD_CORE_ENABLE_QUAZIP "" OFF
    "NOT LD_CORE_RESOURCES_EXTRACTOR" ON)
message(STATUS "LD_CORE_RESOURCES_EXTRACTOR ${LD_CORE_ENABLE_BOX2D}")
message(STATUS "LD_CORE_ENABLE_QUAZIP ${LD_CORE_ENABLE_BOX2D}")

# Qt
# can be done only after project() call!
include(DetectQt)
if(QT_VERSION_MAJOR EQUAL 6)
    if(QT_VERSION_MINOR GREATER_EQUAL 6)
        set(LD_MACOS_DEPLOYMENT_TARGET "11.0")
    elseif(QT_VERSION_MINOR GREATER_EQUAL 5)
        set(LD_MACOS_DEPLOYMENT_TARGET "10.15")
    else()
        set(LD_MACOS_DEPLOYMENT_TARGET "10.14")
    endif()
else()
    set(LD_MACOS_DEPLOYMENT_TARGET "10.13")
endif()


# we need find Qt here because of weird bugs
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Network Multimedia Quick REQUIRED)

if(ANDROID)
    if(QT_VERSION_MAJOR EQUAL 5)
        find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AndroidExtras REQUIRED)
    endif()
endif()

# 3rdparty
if(IOS)
    option(LASERDOCKLIB_USB_SUPPORT "" OFF)
    set(LD_CORE_ENABLE_OPENMP OFF) # OpenMP is not supported on iOS
else()
    mark_as_advanced(LASERDOCKLIB_USB_SUPPORT)
    set(LD_CORE_ENABLE_OPENMP ON)
endif()
message(STATUS "LASERDOCKLIB_USB_SUPPORT ${LASERDOCKLIB_USB_SUPPORT}")
message(STATUS "LD_CORE_ENABLE_OPENMP ${LD_CORE_ENABLE_OPENMP}")

add_subdirectory(3rdparty)

# ldCore
add_subdirectory(ldCore)
add_subdirectory(modules)

# add example for library
if(LD_CORE_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

if(LD_CORE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(CMAKE_GENERATOR STREQUAL Xcode)
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

# just ignore Qt creator annoying default variable
if(DEFINED QT_QMAKE_EXECUTABLE)
    mark_as_advanced(QT_QMAKE_EXECUTABLE)
    set(LD_IGNORED ${QT_QMAKE_EXECUTABLE})
endif()

