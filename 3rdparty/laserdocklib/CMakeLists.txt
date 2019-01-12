cmake_minimum_required(VERSION 3.11)

option(LD_ANDROID_BUILD "Android build" OFF)
if(NOT DEFINED QTDIR)
    set(QTDIR $ENV{QTDIR} CACHE STRING "Path to Qt dir")
endif()

if(LD_ANDROID_BUILD)
    if(NOT DEFINED ANDROID_NDK)
        set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT} CACHE STRING "Path to Android NDK")
    endif()
    if(NOT DEFINED ANDROID_ABI)
        set(ANDROID_ABI "arm64-v8a" CACHE STRING "Android ABI version")  # "arm64-v8a" and "armeabi-v7a" values are supported
    endif()
    if(NOT DEFINED ANDROID_NATIVE_API_LEVEL)
        set(ANDROID_NATIVE_API_LEVEL 21 CACHE STRING "Android native API level")
    endif()

    set(ANDROID_TOOLCHAIN clang)
    set(ANDROID_STL c++_shared)

    set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
endif()

project(laserdocklib)

add_library(laserdocklib SHARED)

target_include_directories(laserdocklib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(laserdocklib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/include/laserdocklib)

target_sources(laserdocklib PRIVATE
    ${INCDIR}/Laserdocklib.h
    ${INCDIR}/LaserdockDevice.h
    ${INCDIR}/LaserdockDeviceManager.h
    ${INCDIR}/LaserdockSample.h
    ${SRCDIR}/LaserdockDevice.cpp
    ${SRCDIR}/LaserdockDevice_p.h
    ${SRCDIR}/LaserdockDeviceManager.cpp
    ${SRCDIR}/LaserdockDeviceManager_p.h
    )

if(ANDROID)
    target_compile_definitions(laserdocklib PRIVATE ANDROID)

    target_sources(laserdocklib PRIVATE
        java/com/wickedlasers/laserdocklib/LaserdockUsbCmdConnection.java
        java/com/wickedlasers/laserdocklib/LaserdockUsbDataConnection.java
        java/com/wickedlasers/laserdocklib/LdUsbDeviceHelper.java
        ${SRCDIR}/jni/ldUsbConnection.cpp
        ${SRCDIR}/jni/ldUsbConnection.h
        ${SRCDIR}/jni/ldUsbDeviceHelper.cpp
        ${SRCDIR}/jni/ldUsbDeviceHelper.h
        ${SRCDIR}/LaserdockDeviceManager_android.cpp
        ${SRCDIR}/LaserdockAndroidUSBDevice.cpp
        ${SRCDIR}/hexdump.cpp
        )
else()
    target_sources(laserdocklib PRIVATE
        ${SRCDIR}/LaserdockDeviceManager_desktop.cpp
        ${SRCDIR}/LaserdockDevice_desktop.cpp
        )
endif()

# Qt
set(QT5_MIN_VERSION 5.11)
set(Qt5_DIR "${QTDIR}/lib/cmake/Qt5")
set(Qt5Core_DIR "${QTDIR}/lib/cmake/Qt5Core")
find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS Core REQUIRED)
target_link_libraries(laserdocklib PRIVATE Qt5::Core )

if(ANDROID)
    set(Qt5AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt5AndroidExtras")
    find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS AndroidExtras REQUIRED)
    target_link_libraries(laserdocklib PRIVATE Qt5::AndroidExtras log)
endif()

# libusb
add_subdirectory(3rdparty/libusb)
target_link_libraries(laserdocklib PUBLIC libusb)


if(NOT WIN32)
    include(GNUInstallDirs)
    set_property(TARGET laserdocklib PROPERTY PUBLIC_HEADER
        ${INCDIR}/Laserdocklib.h
        ${INCDIR}/LaserdockDevice.h
        ${INCDIR}/LaserdockDeviceManager.h
        ${INCDIR}/LaserdockSample.h
        )

    install(TARGETS laserdocklib
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/laserdocklib)
endif()
