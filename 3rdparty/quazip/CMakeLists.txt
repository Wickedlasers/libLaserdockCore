# quazip git@github.com:akforrest/quazip.git

add_library(quazip SHARED IMPORTED GLOBAL)

set_property(TARGET quazip PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

if(WIN32)
#    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#        set(QUAZAIP_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/lib/quazipd.lib")
#    else()
        set(QUAZAIP_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/lib/win/quazip.lib")
        set(QUAZAIP_DLL "${CMAKE_CURRENT_LIST_DIR}/lib/win/quazip.dll")
#    endif()

set_property(TARGET quazip PROPERTY IMPORTED_LOCATION "${QUAZAIP_DLL}")
set_property(TARGET quazip PROPERTY IMPORTED_IMPLIB "${QUAZAIP_LIBRARY}")
endif()

if(APPLE)
    set_property(TARGET quazip PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/mac/libquazip.1.0.0.dylib")
endif()

if(LINUX)
    set_property(TARGET quazip PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/linux/libquazip.so")
endif()

if(ANDROID)
    set_property(TARGET quazip PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/android/${ANDROID_ABI}/libquazip.so")
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::quazip ALIAS quazip)
endif()
