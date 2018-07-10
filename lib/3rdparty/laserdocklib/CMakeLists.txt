# linux detection
if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(LINUX TRUE)
endif()

# sources
set(LASERDOCKLIB_SOURCE_FILES
        LaserdockDevice.cpp
        LaserdockDevice.h
        LaserdockDevice_p.h
        LaserdockDeviceManager.cpp
        LaserdockDeviceManager.h
        LaserdockDeviceManager_p.h
        )

if(ANDROID)
    set(LASERDOCKLIB_SOURCE_FILES
        ${LASERDOCKLIB_SOURCE_FILES}
        java/com/wickedlasers/laserdocklib/LaserdockUsbCmdConnection.java
        java/com/wickedlasers/laserdocklib/LaserdockUsbDataConnection.java
        java/com/wickedlasers/laserdocklib/LdUsbDeviceHelper.java
        jni/ldUsbConnection.cpp
        jni/ldUsbConnection.h
        jni/ldUsbDeviceHelper.cpp
        jni/ldUsbDeviceHelper.h
        LaserdockDeviceManager_android.cpp
#        LaserdockDevice_android.cpp
            LaserdockAndroidUSBDevice.cpp
            hexdump.cpp
            altsettings.c
        )
else()
    set(LASERDOCKLIB_SOURCE_FILES
        ${LASERDOCKLIB_SOURCE_FILES}
        LaserdockDeviceManager_desktop.cpp
        LaserdockDevice_desktop.cpp
        )
endif()

add_library(laserdocklib SHARED ${LASERDOCKLIB_SOURCE_FILES})

# libraries
if(WIN32)
    set(LIBUSB ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb-1.0.lib)
endif()

if(APPLE)
    set(LIBUSB ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb-1.0.0.dylib)
endif()

if(LINUX)
    set(LIBUSB ${CMAKE_CURRENT_SOURCE_DIR}/libusb/libusb-1.0.so)
endif()

if(ANDROID)
    add_definitions(-DANDROID)
    find_package(Qt5 CONFIG REQUIRED Core AndroidExtras)

    if(ANDROID_ABI STREQUAL "armeabi-v7a")
        set(LIBUSB ${CMAKE_CURRENT_SOURCE_DIR}/libusb/android-arm/libusb1.0.so)
    elseif(ANDROID_ABI STREQUAL "x86")
        set(LIBUSB ${CMAKE_CURRENT_SOURCE_DIR}/libusb/android-x86/libusb1.0.so)
    endif()

    set(QT_LIBRARIES Qt5::Core Qt5::AndroidExtras log)
else()
    find_package(Qt5 CONFIG REQUIRED Core)
    set(QT_LIBRARIES Qt5::Core )
endif()

target_link_libraries(laserdocklib ${LIBUSB} ${QT_LIBRARIES})
