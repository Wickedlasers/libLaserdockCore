cmake_minimum_required(VERSION 3.11)

set(CMAKE_AUTOMOC OFF)

add_library(libaudiodecoder)

target_sources(libaudiodecoder PRIVATE
    include/audiodecoder.h
    include/audiodecoderbase.h
    src/audiodecoderbase.cpp
    )

# qdebug only is used
target_link_libraries(libaudiodecoder PRIVATE  Qt5::Core)

if(WIN32)
    target_sources(libaudiodecoder PRIVATE
        include/audiodecodermediafoundation.h
        src/audiodecodermediafoundation.cpp
        )

    target_link_libraries(libaudiodecoder PRIVATE
        Mf.lib
        Mfplat.lib
        Mfreadwrite.lib
        Mfuuid.lib
        )
endif()

if(APPLE)
    target_sources(libaudiodecoder PRIVATE
        include/audiodecodercoreaudio.h
        include/apple/CADebugMacros.h
        include/apple/CAStreamBasicDescription.h
        src/audiodecodercoreaudio.cpp
        )

    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    mark_as_advanced(FOUNDATION_FRAMEWORK
        AUDIOTOOLBOX_FRAMEWORK)

    target_link_libraries(libaudiodecoder PRIVATE
        ${FOUNDATION_FRAMEWORK} ${AUDIOTOOLBOX_FRAMEWORK}
        )

endif()

if(ANDROID)
    target_sources(libaudiodecoder PRIVATE
        include/audiodecoderopensl.h
        src/android/AndroidLogging.h
        src/android/OpenSLDecoder.hpp
        src/android/OpenSLDecoder.cpp
        src/android/OpenSLEngine.hpp
        src/android/OpenSLEngine.cpp
        src/audiodecoderopensl.cpp
        )

    find_library(OPENSLES_LIBRARY
        OpenSLES)
    mark_as_advanced(OPENSLES_LIBRARY)
    target_link_libraries(libaudiodecoder PRIVATE
        ${OPENSLES_LIBRARY})

    target_link_libraries(libaudiodecoder PRIVATE log)
endif()
