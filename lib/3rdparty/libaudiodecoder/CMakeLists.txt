set(CMAKE_AUTOMOC OFF)

# SOURCES
set(LIBAUDIODECODER_SOURCE_FILES
    ${LIBAUDIODECODER_SOURCE_FILES}
    include/audiodecoder.h
    include/audiodecoderbase.h
    src/audiodecoderbase.cpp
    )
# win
if(WIN32)
    set(LIBAUDIODECODER_SOURCE_FILES
        ${LIBAUDIODECODER_SOURCE_FILES}
        include/audiodecodermediafoundation.h
        src/audiodecodermediafoundation.cpp
        )
endif()
# apple
if(APPLE)
    set(LIBAUDIODECODER_SOURCE_FILES
        ${LIBAUDIODECODER_SOURCE_FILES}
        include/audiodecodercoreaudio.h
        include/apple/CADebugMacros.h
        include/apple/CAStreamBasicDescription.h
        src/audiodecodercoreaudio.cpp
        )
endif()

# DEPS
# qdebug only is used
set(LIBAUDIODECODER_LIBRARIES Qt5::Core)
if(WIN32)
    LIST(APPEND LIBAUDIODECODER_LIBRARIES
        Mf.lib
        Mfplat.lib
        Mfreadwrite.lib
        Mfuuid.lib
        )
endif()

if(APPLE)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    mark_as_advanced(FOUNDATION_FRAMEWORK
        AUDIOTOOLBOX_FRAMEWORK)

    LIST(APPEND LIBAUDIODECODER_LIBRARIES
        ${FOUNDATION_FRAMEWORK} ${AUDIOTOOLBOX_FRAMEWORK}
        )

endif()

# LIBRARY
add_library(libaudiodecoder ${LIBAUDIODECODER_SOURCE_FILES})
target_link_libraries(libaudiodecoder ${LIBAUDIODECODER_LIBRARIES} )