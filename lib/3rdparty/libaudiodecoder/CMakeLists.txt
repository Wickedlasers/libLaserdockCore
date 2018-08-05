set(CMAKE_AUTOMOC OFF)

# --------- SOURCES ---------
set(LIBAUDIODECODER_SOURCE_FILES
    ${LIBAUDIODECODER_SOURCE_FILES}
    include/audiodecoder.h
    include/audiodecoderbase.h
    src/audiodecoderbase.cpp
    )

# win
if(WIN32)
    list(APPEND LIBAUDIODECODER_SOURCE_FILES
        include/audiodecodermediafoundation.h
        src/audiodecodermediafoundation.cpp
        )
endif()

# apple
if(APPLE)
    list(APPEND LIBAUDIODECODER_SOURCE_FILES
        include/audiodecodercoreaudio.h
        include/apple/CADebugMacros.h
        include/apple/CAStreamBasicDescription.h
        src/audiodecodercoreaudio.cpp
        )
endif()

if(ANDROID)
    find_library(opensl-lib
        OpenSLES)

    list(APPEND LIBAUDIODECODER_LIBRARIES
        ${opensl-lib})

    list(APPEND LIBAUDIODECODER_SOURCE_FILES
        include/audiodecoderopensl.h
        src/android/AndroidLogging.h
        src/android/OpenSLDecoder.hpp
        src/android/OpenSLDecoder.cpp
        src/android/OpenSLEngine.hpp
        src/android/OpenSLEngine.cpp
        src/audiodecoderopensl.cpp
        )
endif()


# --------- DEPS ---------
# qdebug only is used
list(APPEND LIBAUDIODECODER_LIBRARIES Qt5::Core)
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



# --------- LIBRARY ---------
add_library(libaudiodecoder ${LIBAUDIODECODER_SOURCE_FILES})
target_link_libraries(libaudiodecoder ${LIBAUDIODECODER_LIBRARIES} )
