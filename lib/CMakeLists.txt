# library flags
option(LD_CORE_ENABLE_LIBLZR "Enable liblzr" OFF)
option(LD_CORE_ENABLE_MIDI "Enable midi" OFF)
option(LD_CORE_ENABLE_OPENCV "Enable opencv support" OFF)
option(LD_CORE_ENABLE_QT_QUICK "Enable Qt Quick module" ON) # required for example

# project
project(ldCore)

set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")

if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
    set(LD_BUILD_RELEASE_VERSION ON)
    add_definitions(-DLD_BUILD_RELEASE_VERSION)
endif()

add_definitions(-DLDCORE_LIBRARY)

# platform specific
if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10")
endif()

# ignore warning for unused variable, defined by qt creator
set(IGNORE_VARIABLE "${QT_QMAKE_EXECUTABLE}")

if(LD_CORE_ENABLE_QT_QUICK)
    add_definitions(-DLD_CORE_ENABLE_QT_QUICK)
endif()

# setup C++
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wuninitialized -Winit-self -Wno-unknown-pragmas -Wpedantic")
    if(LINUX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    endif()
    if(NOT ANDROID)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    endif()
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_GENERATOR STREQUAL Xcode)
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------- SOURCES  ----------------------

# sources
set(LDCORELIB_SOURCE_FILES
        ldCore/ldCore_global.h
        ldCore/ldCore.cpp
        ldCore/ldCore.h
        #ldCore/Helpers/
        ldCore/Helpers/ldEnumHelper.h
        ldCore/Helpers/ldLaserController.cpp
        ldCore/Helpers/ldLaserController.h
        ldCore/Helpers/ldRandomGenerator.cpp
        ldCore/Helpers/ldRandomGenerator.h
        #ldCore/Data/
        ldCore/Data/ldAbstractDataWorker.cpp
        ldCore/Data/ldAbstractDataWorker.h
        ldCore/Data/ldBufferManager.cpp
        ldCore/Data/ldBufferManager.h
        ldCore/Data/ldDataDispatcher.cpp
        ldCore/Data/ldDataDispatcher.h
        ldCore/Data/ldFrameBuffer.cpp
        ldCore/Data/ldFrameBuffer.h
        ldCore/Data/ldThreadedDataWorker.cpp
        ldCore/Data/ldThreadedDataWorker.h
        ldCore/Data/ldUsbDataWorker.cpp
        ldCore/Data/ldUsbDataWorker.h
        #ldCore/Filter
        ldCore/Filter/ldBasicFilters.cpp
        ldCore/Filter/ldBasicFilters.h
        ldCore/Filter/ldColorUtils.cpp
        ldCore/Filter/ldColorUtils.h
        ldCore/Filter/ldDeadzoneFilter.cpp
        ldCore/Filter/ldDeadzoneFilter.h
        ldCore/Filter/ldFilter.cpp
        ldCore/Filter/ldFilter.h
        ldCore/Filter/ldFilterBasicData.cpp
        ldCore/Filter/ldFilterBasicData.h
        ldCore/Filter/ldFilterBasicGlobal.cpp
        ldCore/Filter/ldFilterBasicGlobal.h
        ldCore/Filter/ldFilterColorize.cpp
        ldCore/Filter/ldFilterColorize.h
        ldCore/Filter/ldFilterManager.cpp
        ldCore/Filter/ldFilterManager.h
        ldCore/Filter/ldProjectionUtil.cpp
        ldCore/Filter/ldProjectionUtil.h
        # ldCore/Games
        ldCore/Games/ldAbstractGame.cpp
        ldCore/Games/ldAbstractGame.h
        # ldCore/Hardware
        ldCore/Hardware/ldAbstractHardwareManager.cpp
        ldCore/Hardware/ldAbstractHardwareManager.h
        ldCore/Hardware/ldHardware.cpp
        ldCore/Hardware/ldHardware.h
        ldCore/Hardware/ldHardwareManager.cpp
        ldCore/Hardware/ldHardwareManager.h
        ldCore/Hardware/ldUSBHardware.cpp
        ldCore/Hardware/ldUSBHardware.h
        ldCore/Hardware/ldUSBHardwareManager.cpp
        ldCore/Hardware/ldUSBHardwareManager.h
        # ldCore/Render
        ldCore/Render/ldAbstractRenderer.cpp
        ldCore/Render/ldAbstractRenderer.h
        ldCore/Render/ldRendererManager.cpp
        ldCore/Render/ldRendererManager.h
        ldCore/Render/ldRendererOpenlase.cpp
        ldCore/Render/ldRendererOpenlase.h
        #ldCore/Simulator
        ldCore/Simulator/ldSimulatorEngine.cpp
        ldCore/Simulator/ldSimulatorEngine.h
        ldCore/Simulator/ldSimulatorRenderer.cpp
        ldCore/Simulator/ldSimulatorRenderer.h
        #ldCore/Shape
        ldCore/Shape/ldAffineTransform.cpp
        ldCore/Shape/ldAffineTransform.h
        ldCore/Shape/ldEffect.cpp
        ldCore/Shape/ldEffect.h
        ldCore/Shape/ldEffectManager.cpp
        ldCore/Shape/ldEffectManager.h
        ldCore/Shape/ldList.cpp
        ldCore/Shape/ldList.h
        ldCore/Shape/ldParticleGeometry.cpp
        ldCore/Shape/ldParticleGeometry.h
        ldCore/Shape/ldShader.cpp
        ldCore/Shape/ldShader.h
        ldCore/Shape/ldShape.cpp
        ldCore/Shape/ldShape.h
        #ldCore/Sound
        ldCore/Sound/ldAudioDecoder.cpp
        ldCore/Sound/ldAudioDecoder.h
        ldCore/Sound/ldFFT.cpp
        ldCore/Sound/ldFFT.h
        ldCore/Sound/ldQAudioDecoder.cpp
        ldCore/Sound/ldQAudioDecoder.h
        ldCore/Sound/ldQAudioInputDevice.cpp
        ldCore/Sound/ldQAudioInputDevice.h
        ldCore/Sound/ldSoundData.cpp
        ldCore/Sound/ldSoundData.h
        ldCore/Sound/ldSoundDeviceInfo.cpp
        ldCore/Sound/ldSoundDeviceInfo.h
        ldCore/Sound/ldSoundDeviceManager.cpp
        ldCore/Sound/ldSoundDeviceManager.h
        ldCore/Sound/ldSoundInterface.cpp
        ldCore/Sound/ldSoundInterface.h
        ldCore/Sound/ldSoundStubDevice.cpp
        ldCore/Sound/ldSoundStubDevice.h
        #ldCore/Sound/Midi
        ldCore/Sound/Midi/ldMidiManager.cpp
        ldCore/Sound/Midi/ldMidiManager.h
        ldCore/Sound/Midi/ldMidiTypes.cpp
        ldCore/Sound/Midi/ldMidiTypes.h
        #ldCore/Task
        ldCore/Task/ldAbstractTask.cpp
        ldCore/Task/ldAbstractTask.h
        ldCore/Task/ldTaskManager.cpp
        ldCore/Task/ldTaskManager.h
        ldCore/Task/ldTaskWorker.cpp
        ldCore/Task/ldTaskWorker.h
        #ldCore/Utilities
        ldCore/Utilities/ldBasicDataStructures.h
        ldCore/Utilities/ldBasicDataStructures.cpp
        ldCore/Utilities/ldRingBuffer.cpp
        ldCore/Utilities/ldRingBuffer.h
        ldCore/Utilities/ldThreadSafeQueue.h
        ldCore/Utilities/ldUtils.h
        #ldCore/Visualizations
        ldCore/Visualizations/ldLogoLaserdock.cpp
        ldCore/Visualizations/ldLogoLaserdock.h
        ldCore/Visualizations/ldVisualizationTask.cpp
        ldCore/Visualizations/ldVisualizationTask.h
        ldCore/Visualizations/ldVisualizer.cpp
        ldCore/Visualizations/ldVisualizer.h
        #ldCore/Visualizations/MusicManager/
        ldCore/Visualizations/MusicManager/ldMusicManager.cpp
        ldCore/Visualizations/MusicManager/ldMusicManager.h
        #ldCore/Visualizations/util/AudioHelper
        ldCore/Visualizations/util/AudioHelper/ldAppakaBeat.cpp
        ldCore/Visualizations/util/AudioHelper/ldAppakaBeat.h
        ldCore/Visualizations/util/AudioHelper/ldAppakBpmSelector.cpp
        ldCore/Visualizations/util/AudioHelper/ldAppakBpmSelector.h
        ldCore/Visualizations/util/AudioHelper/ldAppakGate.cpp
        ldCore/Visualizations/util/AudioHelper/ldAppakGate.h
        ldCore/Visualizations/util/AudioHelper/ldAppakPeaks.cpp
        ldCore/Visualizations/util/AudioHelper/ldAppakPeaks.h
        ldCore/Visualizations/util/AudioHelper/ldAppakSpectrum.cpp
        ldCore/Visualizations/util/AudioHelper/ldAppakSpectrum.h
        ldCore/Visualizations/util/AudioHelper/ldAudioBasic.cpp
        ldCore/Visualizations/util/AudioHelper/ldAudioBasic.h
        ldCore/Visualizations/util/AudioHelper/ldDurationalStatEstimator.cpp
        ldCore/Visualizations/util/AudioHelper/ldDurationalStatEstimator.h
        ldCore/Visualizations/util/AudioHelper/ldHybridReactor.cpp
        ldCore/Visualizations/util/AudioHelper/ldHybridReactor.h
        ldCore/Visualizations/util/AudioHelper/ldMusicFeature.cpp
        ldCore/Visualizations/util/AudioHelper/ldMusicFeature.h
        ldCore/Visualizations/util/AudioHelper/ldNoiseField.cpp
        ldCore/Visualizations/util/AudioHelper/ldNoiseField.h
        ldCore/Visualizations/util/AudioHelper/ldOnsetDetector.cpp
        ldCore/Visualizations/util/AudioHelper/ldOnsetDetector.h
        ldCore/Visualizations/util/AudioHelper/ldPitchDetector.cpp
        ldCore/Visualizations/util/AudioHelper/ldPitchDetector.h
        ldCore/Visualizations/util/AudioHelper/ldSilentThree.cpp
        ldCore/Visualizations/util/AudioHelper/ldSilentThree.h
        ldCore/Visualizations/util/AudioHelper/ldSoundGate.cpp
        ldCore/Visualizations/util/AudioHelper/ldSoundGate.h
        ldCore/Visualizations/util/AudioHelper/ldSoundEventUtil.cpp
        ldCore/Visualizations/util/AudioHelper/ldSoundEventUtil.h
        ldCore/Visualizations/util/AudioHelper/ldSpectrogram.cpp
        ldCore/Visualizations/util/AudioHelper/ldSpectrogram.h
        ldCore/Visualizations/util/AudioHelper/ldSpectrumFrame.cpp
        ldCore/Visualizations/util/AudioHelper/ldSpectrumFrame.h
        ldCore/Visualizations/util/AudioHelper/ldTempoAC.cpp
        ldCore/Visualizations/util/AudioHelper/ldTempoAC.h
        ldCore/Visualizations/util/AudioHelper/ldTempoTracker.cpp
        ldCore/Visualizations/util/AudioHelper/ldTempoTracker.h
        #ldCore/Visualizations/util/BezierCurveHelper
        ldCore/Visualizations/util/BezierCurveHelper/ld3dBezierCurveFrame.cpp
        ldCore/Visualizations/util/BezierCurveHelper/ld3dBezierCurveFrame.h
        ldCore/Visualizations/util/BezierCurveHelper/ld3dBezierCurveObject.cpp
        ldCore/Visualizations/util/BezierCurveHelper/ld3dBezierCurveObject.h
        ldCore/Visualizations/util/BezierCurveHelper/ldBezierCurveFrame.cpp
        ldCore/Visualizations/util/BezierCurveHelper/ldBezierCurveFrame.h
        ldCore/Visualizations/util/BezierCurveHelper/ldBezierCurveObject.cpp
        ldCore/Visualizations/util/BezierCurveHelper/ldBezierCurveObject.h
        #ldCore/Visualizations/util/ColorHelper
        ldCore/Visualizations/util/ColorHelper/ldColorUtil.cpp
        ldCore/Visualizations/util/ColorHelper/ldColorUtil.h
        ldCore/Visualizations/util/ColorHelper/ldColorUtilMEO.cpp
        ldCore/Visualizations/util/ColorHelper/ldColorUtilMEO.h
        #ldCore/Visualizations/util/DrawHelper
        ldCore/Visualizations/util/DrawHelper/ld3dBezierCurveDrawer.cpp
        ldCore/Visualizations/util/DrawHelper/ld3dBezierCurveDrawer.h
        ldCore/Visualizations/util/DrawHelper/ldBezierCurveDrawer.cpp
        ldCore/Visualizations/util/DrawHelper/ldBezierCurveDrawer.h
        #ldCore/Visualizations/util/MathsHelper
        ldCore/Visualizations/util/MathsHelper/ldGeometryUtil.cpp
        ldCore/Visualizations/util/MathsHelper/ldGeometryUtil.h
        ldCore/Visualizations/util/MathsHelper/ldMathSpectrum.cpp
        ldCore/Visualizations/util/MathsHelper/ldMathSpectrum.h
        ldCore/Visualizations/util/MathsHelper/ldMathStat.cpp
        ldCore/Visualizations/util/MathsHelper/ldMathStat.h
        ldCore/Visualizations/util/MathsHelper/ldMaths.cpp
        ldCore/Visualizations/util/MathsHelper/ldMaths.h
        ldCore/Visualizations/util/MathsHelper/ldSimpleVector.cpp
        ldCore/Visualizations/util/MathsHelper/ldSimpleVector.h
        ldCore/Visualizations/util/MathsHelper/ldSmoothUtil.cpp
        ldCore/Visualizations/util/MathsHelper/ldSmoothUtil.h
        #ldCore/Visualizations/util/SoundHelper
        ldCore/Visualizations/util/SoundHelper/ldQSound.cpp
        ldCore/Visualizations/util/SoundHelper/ldQSound.h
        ldCore/Visualizations/util/SoundHelper/ldSoundEffects.cpp
        ldCore/Visualizations/util/SoundHelper/ldSoundEffects.h
        #ldCore/Visualizations/util/SVGHelper
        ldCore/Visualizations/util/SVGHelper/ldSvgObject.cpp
        ldCore/Visualizations/util/SVGHelper/ldSvgObject.h
        ldCore/Visualizations/util/SVGHelper/ldSvgReader.cpp
        ldCore/Visualizations/util/SVGHelper/ldSvgReader.h
        #ldCore/Visualizations/util/TextHelper
        ldCore/Visualizations/util/TextHelper/ldAbstractText.cpp
        ldCore/Visualizations/util/TextHelper/ldAbstractText.h
        ldCore/Visualizations/util/TextHelper/ld3dTextLabelOne.cpp
        ldCore/Visualizations/util/TextHelper/ld3dTextLabelOne.h
        ldCore/Visualizations/util/TextHelper/ldFont.cpp
        ldCore/Visualizations/util/TextHelper/ldFont.h
        ldCore/Visualizations/util/TextHelper/ldSvgLetter.cpp
        ldCore/Visualizations/util/TextHelper/ldSvgLetter.h
        ldCore/Visualizations/util/TextHelper/ldTextCounterOne.cpp
        ldCore/Visualizations/util/TextHelper/ldTextCounterOne.h
        ldCore/Visualizations/util/TextHelper/ldTextLabel.cpp
        ldCore/Visualizations/util/TextHelper/ldTextLabel.h
        ldCore/Visualizations/util/TextHelper/ldTextSvgHelper.cpp
        ldCore/Visualizations/util/TextHelper/ldTextSvgHelper.h
        #ldCore/Visualizations/util/VisualizerHelper
        ldCore/Visualizations/util/VisualizerHelper/ldAnimTimerUtil.cpp
        ldCore/Visualizations/util/VisualizerHelper/ldAnimTimerUtil.h
        ldCore/Visualizations/util/VisualizerHelper/ldAnimationSequence.cpp
        ldCore/Visualizations/util/VisualizerHelper/ldAnimationSequence.h
        ldCore/Visualizations/util/VisualizerHelper/ldBeatTracker.cpp
        ldCore/Visualizations/util/VisualizerHelper/ldBeatTracker.h
        ldCore/Visualizations/util/VisualizerHelper/ldLaserFilter.cpp
        ldCore/Visualizations/util/VisualizerHelper/ldLaserFilter.h
        ldCore/Visualizations/util/VisualizerHelper/ldLineGraph.cpp
        ldCore/Visualizations/util/VisualizerHelper/ldLineGraph.h
        ldCore/Visualizations/util/VisualizerHelper/ldVisualizerHelper.cpp
        ldCore/Visualizations/util/VisualizerHelper/ldVisualizerHelper.h
        ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.cpp
        ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h
        ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.cpp
        ldCore/Visualizations/Visualizers/Games/Core/ldGameObject.h
        ldCore/Visualizations/Visualizers/Games/Core/ldGameExplosion.cpp
        ldCore/Visualizations/Visualizers/Games/Core/ldGameExplosion.h
        ldCore/Visualizations/Visualizers/Games/Core/ldGameFirework.cpp
        ldCore/Visualizations/Visualizers/Games/Core/ldGameFirework.h
        ldCore/Visualizations/Visualizers/Games/Core/ldGameSmoke.cpp
        ldCore/Visualizations/Visualizers/Games/Core/ldGameSmoke.h
        ldCore/Visualizations/Visualizers/Games/Core/ldGameSparkle.cpp
        ldCore/Visualizations/Visualizers/Games/Core/ldGameSparkle.h
        #Resources
        Resources/laserdock.qrc
        )

# windows
if(WIN32)
    list(APPEND LDCORELIB_SOURCE_FILES
        ldCore/Sound/ldLoopbackAudioDevice.cpp
        ldCore/Sound/ldLoopbackAudioDevice.h
        ldCore/Sound/ldLoopbackAudioDeviceWorker.cpp
        ldCore/Sound/ldLoopbackAudioDeviceWorker.h
        ldCore/Utilities/gettimeofday_win.h
        )
endif()

# qt quick
if(LD_CORE_ENABLE_QT_QUICK)
    list(APPEND LDCORELIB_SOURCE_FILES
        ldCore/Simulator/ldSimulatorItem.cpp
        ldCore/Simulator/ldSimulatorItem.h
        )
endif()

# opencv
if(LD_CORE_ENABLE_OPENCV)
    list(APPEND LDCORELIB_SOURCE_FILES
        #ldCore/Visualizations/util/ImageHelper
        ldCore/Visualizations/util/ImageHelper/ldImageHelper.cpp
        ldCore/Visualizations/util/ImageHelper/ldImageHelper.h
        )
endif()

# liblzr
if(LD_CORE_ENABLE_LIBLZR)
    list(APPEND LDCORELIB_SOURCE_FILES
        ldCore/Helpers/ldLzrHelper.cpp
        ldCore/Helpers/ldLzrHelper.h
        )
endif()

# midi
if(LD_CORE_ENABLE_MIDI)
    list(APPEND LDCORELIB_SOURCE_FILES
        #ldCore/Sound
        ldCore/Sound/ldMidiDevice.cpp
        ldCore/Sound/ldMidiDevice.h
        ldCore/Sound/Midi/ldMidiInfo.cpp
        ldCore/Sound/Midi/ldMidiInfo.h
        ldCore/Sound/Midi/ldMidiInput.cpp
        ldCore/Sound/Midi/ldMidiInput.h
        )

    if(APPLE)
        list(APPEND LDCORELIB_SOURCE_FILES
            ldCore/Sound/Midi/ldMidiInput_mac.mm
            )
    endif()

    if(WIN32)
        list(APPEND LDCORELIB_SOURCE_FILES
            ldCore/Sound/Midi/ldMidiInput_win.cpp
            )
    endif()
endif()

# library
add_library(ldCore SHARED ${LDCORELIB_SOURCE_FILES} )


# ---------------------------- DEPENDENCIES  ----------------------

find_package(Qt5 CONFIG REQUIRED Core Gui Multimedia)
list(APPEND LIBRARIES Qt5::Core Qt5::Gui Qt5::Multimedia)

# Qt Quick (optional)
if(LD_CORE_ENABLE_QT_QUICK)
    set(Qt5Quick_DIR "${QTDIR}/lib/cmake/Qt5Quick")

    find_package(Qt5 CONFIG REQUIRED Quick)
    list(APPEND LIBRARIES Qt5::Quick)
endif()

# 3RDPARTY LIBRARIES
# include root dir for simple access to libraries .h files
include_directories(3rdparty)
# aubio
add_subdirectory(3rdparty/aubio)
list(APPEND LIBRARIES aubio)
# kiss_fft130
add_subdirectory(3rdparty/kiss_fft130)
list(APPEND LIBRARIES kiss_fft130)
# laserdocklib
add_subdirectory(3rdparty/laserdocklib)
list(APPEND LIBRARIES laserdocklib)
# libaudiodecoder
add_subdirectory(3rdparty/libaudiodecoder)
list(APPEND LIBRARIES libaudiodecoder )
# lib-qt-qml-tricks
include_directories(3rdparty/lib-qt-qml-tricks/include)
# liblzr (optional)
if(LD_CORE_ENABLE_LIBLZR)
    add_subdirectory(3rdparty/liblzr)
    list(APPEND LIBRARIES liblzr)
endif()
# nanosvg
# No need to do anything, header-only library
# opencv (optional)
if(LD_CORE_ENABLE_OPENCV)
    include(3rdparty/opencv/CMakeLists.txt)
    list(APPEND LIBRARIES opencv_core opencv_imgproc opencv_highgui opencv_videoio) # opencv_imgcodecs is dependency
endif()
# OpenLase
add_subdirectory(3rdparty/openlase)
list(APPEND LIBRARIES OpenLase)

# platform specific libraries
if(WIN32)
    if(LD_CORE_ENABLE_MIDI)
        list(APPEND LIBRARIES winmm.lib)
    endif()
endif()

if(APPLE)
    find_library(COREMIDI_LIBRARY CoreMIDI)
    list(APPEND LIBRARIES  ${COREMIDI_LIBRARY})
endif()

if(ANDROID)
    list(APPEND LIBRARIES GLESv2)
endif()

# link deps
target_link_libraries(ldCore ${LIBRARIES})


# ---------------------------- EXAMPLE  ----------------------

# add example for library. qt quick module is required
if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR} AND LD_CORE_ENABLE_QT_QUICK)
    add_subdirectory(example)
endif()
