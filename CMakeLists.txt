cmake_minimum_required(VERSION 3.5)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# main app configuration
option(LD_ANDROID_BUILD "Build for Android" OFF)
option(LD_CORE_BUILD_EXAMPLE "Build Example" ON)

# setup C++
set(CMAKE_VERBOSE_MAKEFILE ON)

# setup C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# lib
if(LD_CORE_BUILD_EXAMPLE)
    set(LD_CORE_ENABLE_QT_QUICK ON)
endif()

# try to set QTDIR from environment variable
if(EXISTS $ENV{QTDIR})
    set(QTDIR $ENV{QTDIR} CACHE STRING "Path to Qt dir")
endif()

# android
if(NOT DEFINED LD_ANDROID_BUILD)
    set(LD_ANDROID_BUILD OFF)
endif()

# load local
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
include(local.cmake)
endif()

# set desktop qt dir
if(NOT LD_ANDROID_BUILD)
    if(DEFINED QT_BASE_DIR)
        # win
        if(WIN32)
            set(QTDIR "${QT_BASE_DIR}/msvc2015")
        endif()
        # mac
        if(APPLE)
            set(QTDIR "${QT_BASE_DIR}/clang_64")
        endif()
    endif()
endif()

# Android toolchain: should be before project() call
if(LD_ANDROID_BUILD)
    # Target ABI Qt
    if(DEFINED QT_BASE_DIR)
        if(ANDROID_ABI STREQUAL "armeabi-v7a")
            set(QTDIR "${QT_BASE_DIR}/android_armv7")
        elseif(ANDROID_ABI STREQUAL "x86")
            set(QTDIR "${QT_BASE_DIR}/android_x86")
        endif()
    endif()

    # setup required values for toolchain files if not done already
    if(NOT DEFINED ANDROID_NDK AND EXISTS $ENV{ANDROID_NDK_ROOT})
        set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
    endif()

    set(ANDROID_NATIVE_API_LEVEL 21)

    # setup Android toolchain
    set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/toolchain/android.toolchain.cmake)

    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

message(STATUS "Qt Dir: ${QTDIR}")

# setup Qt_DIR variables (required for android)
set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")
set(Qt5_DIR "${QTDIR}/lib/cmake/Qt5")
set(Qt5AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt5AndroidExtras")
set(Qt5Core_DIR "${QTDIR}/lib/cmake/Qt5Core")
set(Qt5Gui_DIR "${QTDIR}/lib/cmake/Qt5Gui")
set(Qt5Multimedia_DIR "${QTDIR}/lib/cmake/Qt5Multimedia")
set(Qt5Network_DIR "${QTDIR}/lib/cmake/Qt5Network")
set(Qt5OpenGL_DIR "${QTDIR}/lib/cmake/Qt5OpenGL")
set(Qt5Qml_DIR "${QTDIR}/lib/cmake/Qt5Qml")

# project
project(LaserdockCore)

# library
add_subdirectory(lib)

# add example for library. qt quick module is required
if(LD_CORE_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()
