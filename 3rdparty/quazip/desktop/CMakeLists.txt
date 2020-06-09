set_property(TARGET quazip PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")

if(WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(QUAZAIP_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/lib/win/debug/quazip.lib")
        set(QUAZAIP_DLL "${CMAKE_CURRENT_LIST_DIR}/lib/win/debug/quazip.dll")
    else()
        set(QUAZAIP_LIBRARY "${CMAKE_CURRENT_LIST_DIR}/lib/win/release/quazip.lib")
        set(QUAZAIP_DLL "${CMAKE_CURRENT_LIST_DIR}/lib/win/release/quazip.dll")
    endif()

    set_property(TARGET quazip PROPERTY IMPORTED_LOCATION "${QUAZAIP_DLL}")
    set_property(TARGET quazip PROPERTY IMPORTED_IMPLIB "${QUAZAIP_LIBRARY}")
endif()

if(APPLE)
    set_property(TARGET quazip PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/mac/libquazip.1.0.0.dylib")
endif()

if(LINUX)
    set_property(TARGET quazip PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/linux/libquazip.so")
endif()
