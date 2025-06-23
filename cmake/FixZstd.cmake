macro(fix_zstd TARGET)
    # fix ld: error: undefined symbol: qt_resourceFeatureZstd
    if(ANDROID AND QT_VERSION_MAJOR EQUAL 6 AND QT_VERSION_MINOR EQUAL 6)
        set_property(TARGET ${TARGET} APPEND PROPERTY AUTORCC_OPTIONS "--no-zstd")
    endif()
endmacro()
