cmake_minimum_required(VERSION 3.11)

option(LD_ANDROID_BUILD "Android build" OFF)
option(LASERDOCKLIB_USB_SUPPORT "Support for USB devices" ON)
option(LASERDOCKLIB_INSTALL "Install laserdocklib" OFF)

if((EXISTS $ENV{QTDIR}) AND (NOT DEFINED QTDIR))
    set(QTDIR $ENV{QTDIR} CACHE STRING "Path to Qt dir")
endif()


if(NOT DEFINED ANDROID_NDK)
    set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT} CACHE STRING "Path to Android NDK")
endif()
if(NOT DEFINED ANDROID_ABI)
    set(ANDROID_ABI "arm64-v8a" CACHE STRING "Android ABI version")  # "arm64-v8a" and "armeabi-v7a" values are supported
endif()
if(NOT DEFINED ANDROID_NATIVE_API_LEVEL)
    set(ANDROID_NATIVE_API_LEVEL 24 CACHE STRING "Android native API level")
endif()

set(ANDROID_TOOLCHAIN clang)
set(ANDROID_STL c++_shared)

if(LD_ANDROID_BUILD)
    set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
endif()

if(IOS)
    add_library(laserdocklib)
else()
    add_library(laserdocklib SHARED)
endif()

target_include_directories(laserdocklib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(laserdocklib PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/include/laserdocklib)

target_sources(laserdocklib PRIVATE
    ${INCDIR}/LaserdockSample.h
    ${INCDIR}/Laserdocklib.h
    ${INCDIR}/LaserDockNetworkDevice.h
    ${SRCDIR}/LaserDockNetworkDevice.cpp
    ${INCDIR}/ldSharedQueue.h
    )

if(ANDROID)
    target_compile_definitions(laserdocklib PRIVATE ANDROID)
endif()

# Qt
if(QT_VERSION_MAJOR EQUAL 6)
    set(QT_MIN_VERSION 6.6)
else()
    set(QT_MIN_VERSION 5.15)
endif()

set(Qt${QT_VERSION_MAJOR}_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}")
set(Qt${QT_VERSION_MAJOR}Core_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Core")
find_package(Qt${QT_VERSION_MAJOR} ${QT_MIN_VERSION} COMPONENTS Core Network REQUIRED)
target_link_libraries(laserdocklib PRIVATE Qt::Core Qt::Network)

if(ANDROID)

    target_sources(laserdocklib PRIVATE
        ${INCDIR}/ldAndroidGlobals.h
        )

    if(QT_VERSION_MAJOR EQUAL 6)
        target_link_libraries(laserdocklib PRIVATE Qt6::CorePrivate)
    else()
        set(Qt5AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}AndroidExtras")
        find_package(Qt5 ${QT_MIN_VERSION} COMPONENTS AndroidExtras REQUIRED)
        target_link_libraries(laserdocklib PRIVATE Qt::AndroidExtras)
    endif()

    target_link_libraries(laserdocklib PRIVATE log)

endif()

if(WIN32)
    target_compile_definitions(laserdocklib PRIVATE LASERDOCKLIB_LIBRARY)
    target_compile_options(laserdocklib PRIVATE /wd4251)
endif()

if(QT_VERSION_MAJOR EQUAL 5 AND IOS)
    target_link_libraries(laserdocklib PRIVATE z)
endif()

# -------------------- LASERDOCKLIB_USB_SUPPORT ----------------

if(LASERDOCKLIB_USB_SUPPORT)
    target_compile_definitions(laserdocklib PUBLIC LASERDOCKLIB_USB_SUPPORT)

    target_sources(laserdocklib PRIVATE
        ${INCDIR}/LaserdockDevice.h
        ${INCDIR}/LaserdockDeviceManager.h
        ${SRCDIR}/LaserdockDevice.cpp
        ${SRCDIR}/LaserdockDevice_p.h
        ${SRCDIR}/LaserdockDeviceManager.cpp
        ${SRCDIR}/LaserdockDeviceManager_p.h
        )

    if(ANDROID)
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

    # libusb
    add_subdirectory(3rdparty/libusb)
    target_link_libraries(laserdocklib PRIVATE libusb)
endif()

# -------------------- LASERDOCKLIB_INSTALL ----------------

if(LASERDOCKLIB_INSTALL)
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
endif()

# -------------------- Export with alias ----------------

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::laserdocklib ALIAS laserdocklib)
endif()

