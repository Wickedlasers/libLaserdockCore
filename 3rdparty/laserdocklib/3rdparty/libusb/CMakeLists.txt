cmake_minimum_required(VERSION 3.11)

# linux detection
if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(LINUX TRUE)
endif()

add_library(libusb SHARED IMPORTED GLOBAL)

set_property(TARGET libusb PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/include)

if(WIN32)
    set_target_properties(libusb PROPERTIES
        IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/win/libusb-1.0.dll"
        IMPORTED_IMPLIB "${CMAKE_CURRENT_LIST_DIR}/lib/win/libusb-1.0.lib"
        )
endif()

if(APPLE)
    set_property(TARGET libusb PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/mac/libusb-1.0.0.dylib)
endif()

if(LINUX)
    set_property(TARGET libusb PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/linux/libusb-1.0.so)
endif()

if(ANDROID)
    set_property(TARGET libusb PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/android/${ANDROID_ABI}/libusb1.0.so)
endif()
