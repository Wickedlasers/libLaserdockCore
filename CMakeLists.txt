cmake_minimum_required(VERSION 3.11)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# main app configuration
option(LD_ANDROID_BUILD "Build for Android" OFF)
option(LD_CORE_BUILD_EXAMPLE "Build Example" ON)
option(LD_CORE_BUILD_TESTS "Build Tests" OFF)
# library flags
option(LD_CORE_ENABLE_LIBLZR "Enable liblzr" OFF)
option(LD_CORE_ENABLE_MIDI "Enable midi" OFF)
option(LD_CORE_ENABLE_OPENCV "Enable opencv support" OFF)
option(LD_CORE_ENABLE_QT_QUICK "Enable Qt Quick module" OFF) # required for example
option(LD_CORE_GAMES_ALWAYS_PLAY_STATE "Games play after pause automatically" OFF)
option(LD_CORE_SHOW_LOGO "Show Laserdock logo on start" ON)
option(LD_CORE_REDUCE_ANALYZER_SUPPORT "Reduce music analyzer support for performance on slow devices" OFF)

include(CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(LD_CORE_ENABLE_QT_QUICK "Enable Qt Quick module" OFF
                       "NOT LD_CORE_BUILD_EXAMPLE" ON)

if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11")
endif()

if ((CMAKE_BUILD_TYPE STREQUAL "Release") OR (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo") OR (CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
    set(LD_BUILD_RELEASE_VERSION ON)
endif()


# setup C++
set(CMAKE_VERBOSE_MAKEFILE ON)

# setup C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# try to set QTDIR from environment variable
if(EXISTS $ENV{QTDIR})
    set(QTDIR $ENV{QTDIR} CACHE STRING "Path to Qt dir")
endif()

# android
if(NOT DEFINED LD_ANDROID_BUILD)
    set(LD_ANDROID_BUILD OFF)
endif()

# load local
include(local.cmake OPTIONAL)

# set desktop qt dir
if(DEFINED QT_BASE_DIR AND NOT DEFINED QTDIR)
    if(LD_ANDROID_BUILD )
        if(ANDROID_ABI STREQUAL "armeabi-v7a")
            set(QTDIR "${QT_BASE_DIR}/android_armv7")
        elseif(ANDROID_ABI STREQUAL "x86")
            set(QTDIR "${QT_BASE_DIR}/android_x86")
        elseif(ANDROID_ABI STREQUAL "arm64-v8a")
            set(QTDIR "${QT_BASE_DIR}/android_arm64_v8a")
        endif()
    else()
        # win
        if(WIN32)
            set(QTDIR "${QT_BASE_DIR}/msvc2015")
        endif()
        # mac
        if(APPLE)
            set(QTDIR "${QT_BASE_DIR}/clang_64")
        endif()
    endif()
endif()
message(STATUS "Qt Dir: ${QTDIR}")

# Android toolchain: should be before project() call
if(LD_ANDROID_BUILD)
    # setup required values for toolchain files if not done already
    if(NOT DEFINED ANDROID_NDK AND EXISTS $ENV{ANDROID_NDK_ROOT})
        set(ANDROID_NDK $ENV{ANDROID_NDK_ROOT})
    endif()

    set(ANDROID_NATIVE_API_LEVEL 21)
    set(ANDROID_TOOLCHAIN clang)
    set(ANDROID_STL c++_shared)

    # setup Android toolchain
    set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)

    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "CMAKE_SHARED_LINKER_FLAGS: ${CMAKE_SHARED_LINKER_FLAGS}")
endif()

# setup Qt_DIR variables (required for android)
set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")
set(Qt5_DIR "${QTDIR}/lib/cmake/Qt5")
set(Qt5AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt5AndroidExtras")
set(Qt5Core_DIR "${QTDIR}/lib/cmake/Qt5Core")
set(Qt5Gamepad_DIR "${QTDIR}/lib/cmake/Qt5Gamepad")
set(Qt5Gui_DIR "${QTDIR}/lib/cmake/Qt5Gui")
set(Qt5Multimedia_DIR "${QTDIR}/lib/cmake/Qt5Multimedia")
set(Qt5Network_DIR "${QTDIR}/lib/cmake/Qt5Network")
set(Qt5OpenGL_DIR "${QTDIR}/lib/cmake/Qt5OpenGL")
set(Qt5Qml_DIR "${QTDIR}/lib/cmake/Qt5Qml")
set(Qt5Test_DIR "${QTDIR}/lib/cmake/Qt5Test")

# project
project(ldCore)

if(CMAKE_GENERATOR STREQUAL Xcode)
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++11")
    SET(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# library
add_library(${PROJECT_NAME} SHARED )

set(INCDIR include/ldCore)
set(SRCDIR src)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

# sources
target_sources(${PROJECT_NAME} PRIVATE
        ${INCDIR}/ldCore_global.h
        ${SRCDIR}/ldCore.cpp
        ${INCDIR}/ldCore.h
        #Helpers/
        ${INCDIR}/Helpers/ldEnumHelper.h
        ${SRCDIR}/Helpers/ldLaserController.cpp
        ${INCDIR}/Helpers/ldLaserController.h
        ${SRCDIR}/Helpers/ldRandomGenerator.cpp
        ${INCDIR}/Helpers/ldRandomGenerator.h
        #Data/
        ${SRCDIR}/Data/ldAbstractDataWorker.cpp
        ${INCDIR}/Data/ldAbstractDataWorker.h
        ${SRCDIR}/Data/ldBufferManager.cpp
        ${INCDIR}/Data/ldBufferManager.h
        ${SRCDIR}/Data/ldDataDispatcher.cpp
        ${INCDIR}/Data/ldDataDispatcher.h
        ${SRCDIR}/Data/ldFrameBuffer.cpp
        ${INCDIR}/Data/ldFrameBuffer.h
        ${SRCDIR}/Data/ldThreadedDataWorker.cpp
        ${SRCDIR}/Data/ldThreadedDataWorker.h
        ${SRCDIR}/Data/ldUsbDataWorker.cpp
        ${INCDIR}/Data/ldUsbDataWorker.h
        #Filter
        ${SRCDIR}/Filter/ldBasicFilters.cpp
        ${INCDIR}/Filter/ldBasicFilters.h
        ${SRCDIR}/Filter/ldDeadzoneFilter.cpp
        ${INCDIR}/Filter/ldDeadzoneFilter.h
        ${SRCDIR}/Filter/ldFilter.cpp
        ${INCDIR}/Filter/ldFilter.h
        ${SRCDIR}/Filter/ldFilterBasicData.cpp
        ${INCDIR}/Filter/ldFilterBasicData.h
        ${SRCDIR}/Filter/ldFilterBasicGlobal.cpp
        ${INCDIR}/Filter/ldFilterBasicGlobal.h
        ${SRCDIR}/Filter/ldFilterColorize.cpp
        ${INCDIR}/Filter/ldFilterColorize.h
        ${SRCDIR}/Filter/ldFilterManager.cpp
        ${INCDIR}/Filter/ldFilterManager.h
        ${SRCDIR}/Filter/ldProjectionUtil.cpp
        ${INCDIR}/Filter/ldProjectionUtil.h
        ${SRCDIR}/Filter/ldSoundLevelFilter.cpp
        ${INCDIR}/Filter/ldSoundLevelFilter.h
        #Games
        ${SRCDIR}/Games/ldAbstractGame.cpp
        ${INCDIR}/Games/ldAbstractGame.h
        ${SRCDIR}/Games/ldGamepad.cpp
        ${INCDIR}/Games/ldGamepad.h
        ${SRCDIR}/Games/ldGamepadCtrl.cpp
        ${INCDIR}/Games/ldGamepadCtrl.h
        #Hardware
        ${SRCDIR}/Hardware/ldAbstractHardwareManager.cpp
        ${INCDIR}/Hardware/ldAbstractHardwareManager.h
        ${SRCDIR}/Hardware/ldHardware.cpp
        ${INCDIR}/Hardware/ldHardware.h
        ${SRCDIR}/Hardware/ldHardwareManager.cpp
        ${INCDIR}/Hardware/ldHardwareManager.h
        ${SRCDIR}/Hardware/ldUSBHardware.cpp
        ${INCDIR}/Hardware/ldUSBHardware.h
        ${SRCDIR}/Hardware/ldUSBHardwareManager.cpp
        ${INCDIR}/Hardware/ldUSBHardwareManager.h
        #Render
        ${SRCDIR}/Render/ldAbstractRenderer.cpp
        ${INCDIR}/Render/ldAbstractRenderer.h
        ${SRCDIR}/Render/ldRendererManager.cpp
        ${INCDIR}/Render/ldRendererManager.h
        ${SRCDIR}/Render/ldRendererOpenlase.cpp
        ${INCDIR}/Render/ldRendererOpenlase.h
        #Simulator
        ${SRCDIR}/Simulator/ldSimulatorEngine.cpp
        ${INCDIR}/Simulator/ldSimulatorEngine.h
        ${SRCDIR}/Simulator/ldSimulatorRenderer.cpp
        ${INCDIR}/Simulator/ldSimulatorRenderer.h
        #Shape
        ${SRCDIR}/Shape/ldAffineTransform.cpp
        ${INCDIR}/Shape/ldAffineTransform.h
        ${SRCDIR}/Shape/ldEffect.cpp
        ${INCDIR}/Shape/ldEffect.h
        ${SRCDIR}/Shape/ldEffectManager.cpp
        ${INCDIR}/Shape/ldEffectManager.h
        ${SRCDIR}/Shape/ldList.cpp
        ${INCDIR}/Shape/ldList.h
        ${SRCDIR}/Shape/ldParticleGeometry.cpp
        ${INCDIR}/Shape/ldParticleGeometry.h
        ${SRCDIR}/Shape/ldShader.cpp
        ${INCDIR}/Shape/ldShader.h
        ${SRCDIR}/Shape/ldShape.cpp
        ${INCDIR}/Shape/ldShape.h
        #Sound
        ${SRCDIR}/Sound/ldAudioDecoder.cpp
        ${INCDIR}/Sound/ldAudioDecoder.h
        ${SRCDIR}/Sound/ldFFT.cpp
        ${SRCDIR}/Sound/ldFFT.h
        ${SRCDIR}/Sound/ldQAudioInputDevice.cpp
        ${SRCDIR}/Sound/ldQAudioInputDevice.h
        ${SRCDIR}/Sound/ldSoundData.cpp
        ${INCDIR}/Sound/ldSoundData.h
        ${SRCDIR}/Sound/ldSoundDeviceInfo.cpp
        ${INCDIR}/Sound/ldSoundDeviceInfo.h
        ${SRCDIR}/Sound/ldSoundDeviceManager.cpp
        ${INCDIR}/Sound/ldSoundDeviceManager.h
        ${SRCDIR}/Sound/ldSoundInterface.cpp
        ${INCDIR}/Sound/ldSoundInterface.h
        ${SRCDIR}/Sound/ldSoundStubDevice.cpp
        ${SRCDIR}/Sound/ldSoundStubDevice.h
        #Sound/Midi
        ${SRCDIR}/Sound/Midi/ldMidiManager.cpp
        ${INCDIR}/Sound/Midi/ldMidiManager.h
        ${SRCDIR}/Sound/Midi/ldMidiTypes.cpp
        ${INCDIR}/Sound/Midi/ldMidiTypes.h
        #Task
        ${SRCDIR}/Task/ldAbstractTask.cpp
        ${INCDIR}/Task/ldAbstractTask.h
        ${SRCDIR}/Task/ldTaskManager.cpp
        ${INCDIR}/Task/ldTaskManager.h
        ${SRCDIR}/Task/ldTaskWorker.cpp
        ${INCDIR}/Task/ldTaskWorker.h
        #Utilities
        ${INCDIR}/Utilities/ldBasicDataStructures.h
        ${SRCDIR}/Utilities/ldBasicDataStructures.cpp
        ${SRCDIR}/Utilities/ldRingBuffer.cpp
        ${SRCDIR}/Utilities/ldRingBuffer.h
        ${INCDIR}/Utilities/ldThreadSafeQueue.h
        ${INCDIR}/Utilities/ldUtils.h
        #Visualizations
        ${SRCDIR}/Visualizations/ldLogoLaserdock.cpp
        ${INCDIR}/Visualizations/ldLogoLaserdock.h
        ${SRCDIR}/Visualizations/ldVisualizationTask.cpp
        ${INCDIR}/Visualizations/ldVisualizationTask.h
        ${SRCDIR}/Visualizations/ldVisualizer.cpp
        ${INCDIR}/Visualizations/ldVisualizer.h
        #Visualizations/MusicManager/
        ${SRCDIR}/Visualizations/MusicManager/ldMusicManager.cpp
        ${INCDIR}/Visualizations/MusicManager/ldMusicManager.h
        #Helpers/Audio
        ${SRCDIR}/Helpers/Audio/ldAppakaBeat.cpp
        ${INCDIR}/Helpers/Audio/ldAppakaBeat.h
        ${SRCDIR}/Helpers/Audio/ldAppakBpmSelector.cpp
        ${INCDIR}/Helpers/Audio/ldAppakBpmSelector.h
        ${SRCDIR}/Helpers/Audio/ldAppakGate.cpp
        ${INCDIR}/Helpers/Audio/ldAppakGate.h
        ${SRCDIR}/Helpers/Audio/ldAppakPeaks.cpp
        ${INCDIR}/Helpers/Audio/ldAppakPeaks.h
        ${SRCDIR}/Helpers/Audio/ldAppakSpectrum.cpp
        ${INCDIR}/Helpers/Audio/ldAppakSpectrum.h
        ${SRCDIR}/Helpers/Audio/ldAudioBasic.cpp
        ${INCDIR}/Helpers/Audio/ldAudioBasic.h
        ${SRCDIR}/Helpers/Audio/ldDurationalStatEstimator.cpp
        ${INCDIR}/Helpers/Audio/ldDurationalStatEstimator.h
        ${SRCDIR}/Helpers/Audio/ldHybridReactor.cpp
        ${INCDIR}/Helpers/Audio/ldHybridReactor.h
        ${SRCDIR}/Helpers/Audio/ldMusicFeature.cpp
        ${INCDIR}/Helpers/Audio/ldMusicFeature.h
        ${SRCDIR}/Helpers/Audio/ldNoiseField.cpp
        ${INCDIR}/Helpers/Audio/ldNoiseField.h
        ${SRCDIR}/Helpers/Audio/ldOnsetDetector.cpp
        ${INCDIR}/Helpers/Audio/ldOnsetDetector.h
        ${SRCDIR}/Helpers/Audio/ldPitchDetector.cpp
        ${INCDIR}/Helpers/Audio/ldPitchDetector.h
        ${SRCDIR}/Helpers/Audio/ldSilentThree.cpp
        ${INCDIR}/Helpers/Audio/ldSilentThree.h
        ${SRCDIR}/Helpers/Audio/ldSoundGate.cpp
        ${INCDIR}/Helpers/Audio/ldSoundGate.h
        ${SRCDIR}/Helpers/Audio/ldSoundEventUtil.cpp
        ${INCDIR}/Helpers/Audio/ldSoundEventUtil.h
        ${SRCDIR}/Helpers/Audio/ldSpectrogram.cpp
        ${INCDIR}/Helpers/Audio/ldSpectrogram.h
        ${SRCDIR}/Helpers/Audio/ldSpectrumFrame.cpp
        ${INCDIR}/Helpers/Audio/ldSpectrumFrame.h
        ${SRCDIR}/Helpers/Audio/ldTempoAC.cpp
        ${INCDIR}/Helpers/Audio/ldTempoAC.h
        ${SRCDIR}/Helpers/Audio/ldTempoTracker.cpp
        ${INCDIR}/Helpers/Audio/ldTempoTracker.h
        #Helpers/BezierCurve
        ${SRCDIR}/Helpers/BezierCurve/ld3dBezierCurve.cpp
        ${INCDIR}/Helpers/BezierCurve/ld3dBezierCurve.h
        ${SRCDIR}/Helpers/BezierCurve/ld3dBezierCurveFrame.cpp
        ${INCDIR}/Helpers/BezierCurve/ld3dBezierCurveFrame.h
        ${SRCDIR}/Helpers/BezierCurve/ld3dBezierCurveObject.cpp
        ${INCDIR}/Helpers/BezierCurve/ld3dBezierCurveObject.h
        ${SRCDIR}/Helpers/BezierCurve/ldBezierCurve.cpp
        ${INCDIR}/Helpers/BezierCurve/ldBezierCurve.h
        ${SRCDIR}/Helpers/BezierCurve/ldBezierCurveFrame.cpp
        ${INCDIR}/Helpers/BezierCurve/ldBezierCurveFrame.h
        ${SRCDIR}/Helpers/BezierCurve/ldBezierCurveObject.cpp
        ${INCDIR}/Helpers/BezierCurve/ldBezierCurveObject.h
        ${SRCDIR}/Helpers/BezierCurve/ldBezierPath.cpp
        ${INCDIR}/Helpers/BezierCurve/ldBezierPath.h
        #Helpers/Color
        ${SRCDIR}/Helpers/Color/ldColorEffects.cpp
        ${INCDIR}/Helpers/Color/ldColorEffects.h
        ${SRCDIR}/Helpers/Color/ldColorUtil.cpp
        ${INCDIR}/Helpers/Color/ldColorUtil.h
        ${SRCDIR}/Helpers/Color/ldGradient.cpp
        ${INCDIR}/Helpers/Color/ldGradient.h
        #Helpers/Draw
        ${SRCDIR}/Helpers/Draw/ld3dBezierCurveDrawer.cpp
        ${INCDIR}/Helpers/Draw/ld3dBezierCurveDrawer.h
        ${SRCDIR}/Helpers/Draw/ldBezierCurveDrawer.cpp
        ${INCDIR}/Helpers/Draw/ldBezierCurveDrawer.h
        #Helpers/Maths
        ${INCDIR}/Helpers/Maths/ldAnimatedAngle.h
        ${SRCDIR}/Helpers/Maths/ldGeometryUtil.cpp
        ${INCDIR}/Helpers/Maths/ldGeometryUtil.h
        ${SRCDIR}/Helpers/Maths/ldMathSpectrum.cpp
        ${INCDIR}/Helpers/Maths/ldMathSpectrum.h
        ${SRCDIR}/Helpers/Maths/ldMathStat.cpp
        ${INCDIR}/Helpers/Maths/ldMathStat.h
        ${SRCDIR}/Helpers/Maths/ldMaths.cpp
        ${INCDIR}/Helpers/Maths/ldMaths.h
        ${SRCDIR}/Helpers/Maths/ldRect.cpp
        ${INCDIR}/Helpers/Maths/ldRect.h
        ${SRCDIR}/Helpers/Maths/ldRect3.cpp
        ${INCDIR}/Helpers/Maths/ldRect3.h
        ${SRCDIR}/Helpers/Maths/ldSimpleVector.cpp
        ${INCDIR}/Helpers/Maths/ldSimpleVector.h
        ${SRCDIR}/Helpers/Maths/ldSmoothUtil.cpp
        ${INCDIR}/Helpers/Maths/ldSmoothUtil.h
        ${SRCDIR}/Helpers/Maths/ldVec2.cpp
        ${INCDIR}/Helpers/Maths/ldVec2.h
        ${SRCDIR}/Helpers/Maths/ldVec3.cpp
        ${INCDIR}/Helpers/Maths/ldVec3.h
        #Helpers/Sound
        ${SRCDIR}/Helpers/Sound/ldQSound.cpp
        ${INCDIR}/Helpers/Sound/ldQSound.h
        ${SRCDIR}/Helpers/Sound/ldSoundEffects.cpp
        ${INCDIR}/Helpers/Sound/ldSoundEffects.h
        #Helpers/SVG
        ${SRCDIR}/Helpers/SVG/ldSvgDir.cpp
        ${INCDIR}/Helpers/SVG/ldSvgDir.h
        ${SRCDIR}/Helpers/SVG/ldSvgObject.cpp
        ${INCDIR}/Helpers/SVG/ldSvgObject.h
        ${SRCDIR}/Helpers/SVG/ldSvgReader.cpp
        ${INCDIR}/Helpers/SVG/ldSvgReader.h
        #Helpers/Text
        ${SRCDIR}/Helpers/Text/ldAbstractText.cpp
        ${INCDIR}/Helpers/Text/ldAbstractText.h
        ${SRCDIR}/Helpers/Text/ld3dTextLabelOne.cpp
        ${INCDIR}/Helpers/Text/ld3dTextLabelOne.h
        ${SRCDIR}/Helpers/Text/ldFont.cpp
        ${INCDIR}/Helpers/Text/ldFont.h
        ${SRCDIR}/Helpers/Text/ldSvgLetter.cpp
        ${INCDIR}/Helpers/Text/ldSvgLetter.h
        ${SRCDIR}/Helpers/Text/ldTextCounterOne.cpp
        ${INCDIR}/Helpers/Text/ldTextCounterOne.h
        ${SRCDIR}/Helpers/Text/ldTextLabel.cpp
        ${INCDIR}/Helpers/Text/ldTextLabel.h
        ${SRCDIR}/Helpers/Text/ldTextSvgHelper.cpp
        ${INCDIR}/Helpers/Text/ldTextSvgHelper.h
        #Helpers/Visualizer
        ${SRCDIR}/Helpers/Visualizer/ldAnimTimerUtil.cpp
        ${INCDIR}/Helpers/Visualizer/ldAnimTimerUtil.h
        ${SRCDIR}/Helpers/Visualizer/ldAnimationSequence.cpp
        ${INCDIR}/Helpers/Visualizer/ldAnimationSequence.h
        ${SRCDIR}/Helpers/Visualizer/ldBeatTracker.cpp
        ${INCDIR}/Helpers/Visualizer/ldBeatTracker.h
        ${SRCDIR}/Helpers/Visualizer/ldLaserFilter.cpp
        ${INCDIR}/Helpers/Visualizer/ldLaserFilter.h
        ${SRCDIR}/Helpers/Visualizer/ldLineGraph.cpp
        ${INCDIR}/Helpers/Visualizer/ldLineGraph.h
        ${SRCDIR}/Helpers/Visualizer/ldVisualizerHelper.cpp
        ${INCDIR}/Helpers/Visualizer/ldVisualizerHelper.h
        ${SRCDIR}/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.cpp
        ${INCDIR}/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h
        ${SRCDIR}/Visualizations/Visualizers/Games/Core/ldGameObject.cpp
        ${INCDIR}/Visualizations/Visualizers/Games/Core/ldGameObject.h
        ${SRCDIR}/Visualizations/Visualizers/Games/Core/ldGameExplosion.cpp
        ${INCDIR}/Visualizations/Visualizers/Games/Core/ldGameExplosion.h
        ${SRCDIR}/Visualizations/Visualizers/Games/Core/ldGameFirework.cpp
        ${INCDIR}/Visualizations/Visualizers/Games/Core/ldGameFirework.h
        ${SRCDIR}/Visualizations/Visualizers/Games/Core/ldGameSmoke.cpp
        ${INCDIR}/Visualizations/Visualizers/Games/Core/ldGameSmoke.h
        ${SRCDIR}/Visualizations/Visualizers/Games/Core/ldGameSparkle.cpp
        ${INCDIR}/Visualizations/Visualizers/Games/Core/ldGameSparkle.h
        #Resources
        res/laserdock.qrc
        )

# windows
if(WIN32)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRCDIR}/Sound/ldLoopbackAudioDevice.cpp
        ${SRCDIR}/Sound/ldLoopbackAudioDevice.h
        ${SRCDIR}/Sound/ldLoopbackAudioDeviceWorker.cpp
        ${SRCDIR}/Sound/ldLoopbackAudioDeviceWorker.h
        )
endif()

# opencv
if(LD_CORE_ENABLE_OPENCV)
    target_sources(${PROJECT_NAME} PRIVATE
        #Helpers/Image
        ${SRCDIR}/Helpers/Image/ldImageHelper.cpp
        ${INCDIR}/Helpers/Image/ldImageHelper.h
        )
endif()

# liblzr
if(LD_CORE_ENABLE_LIBLZR)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRCDIR}/Helpers/ldLzrHelper.cpp
        ${INCDIR}/Helpers/ldLzrHelper.h
        )
endif()

# midi
if(LD_CORE_ENABLE_MIDI)
    target_sources(${PROJECT_NAME} PRIVATE
        #Sound
        ${SRCDIR}/Sound/ldMidiDevice.cpp
        ${SRCDIR}/Sound/ldMidiDevice.h
        ${SRCDIR}/Sound/Midi/ldMidiInfo.cpp
        ${INCDIR}/Sound/Midi/ldMidiInfo.h
        ${SRCDIR}/Sound/Midi/ldMidiInput.cpp
        ${INCDIR}/Sound/Midi/ldMidiInput.h
        )

    if(APPLE)
        target_sources(${PROJECT_NAME} PRIVATE
            ${SRCDIR}/Sound/Midi/ldMidiInput_mac.mm
            )
    endif()

    if(WIN32)
        target_sources(${PROJECT_NAME} PRIVATE
            ${SRCDIR}/Sound/Midi/ldMidiInput_win.cpp
            )
    endif()
endif()

# setup C++
if(UNIX)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wuninitialized -Winit-self -Wno-unknown-pragmas -Wpedantic)
    if(LINUX)
        target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
    endif()
    if(NOT ANDROID)
        target_compile_options(${PROJECT_NAME} PRIVATE -Wshadow)
    endif()
endif()

if(LD_CORE_ENABLE_MIDI)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LD_CORE_ENABLE_MIDI)
endif()

if(LD_CORE_REDUCE_ANALYZER_SUPPORT)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LD_CORE_REDUCE_ANALYZER_SUPPORT)
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE LDCORE_LIBRARY)

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC _USE_MATH_DEFINES)
endif()

if(LD_CORE_SHOW_LOGO)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_CORE_SHOW_LOGO)
endif()

if(LD_BUILD_RELEASE_VERSION)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_BUILD_RELEASE_VERSION)
endif()

if(LD_CORE_GAMES_ALWAYS_PLAY_STATE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_CORE_GAMES_ALWAYS_PLAY_STATE)
endif()


# ---------------------------- DEPENDENCIES  ----------------------

set(QT5_MIN_VERSION 5.11)
find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS Core Gui Multimedia Gamepad REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Gamepad)

# Qt Quick (optional)
if(LD_CORE_ENABLE_QT_QUICK)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRCDIR}/Simulator/ldSimulatorItem.cpp
        ${INCDIR}/Simulator/ldSimulatorItem.h
        )

    set(Qt5Quick_DIR "${QTDIR}/lib/cmake/Qt5Quick")

    find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS Quick REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Quick)

    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_CORE_ENABLE_QT_QUICK)
endif()

# 3RDPARTY LIBRARIES
# include root dir for simple access to libraries .h files
target_include_directories(${PROJECT_NAME} PUBLIC 3rdparty)
# aubio
add_subdirectory(3rdparty/aubio)
target_link_libraries(${PROJECT_NAME} PRIVATE aubio)
# kiss_fft130
add_subdirectory(3rdparty/kiss_fft130)
target_link_libraries(${PROJECT_NAME} PRIVATE kiss_fft130)
# laserdocklib
add_subdirectory(3rdparty/laserdocklib)
target_link_libraries(${PROJECT_NAME} PRIVATE laserdocklib)
# libaudiodecoder
add_subdirectory(3rdparty/libaudiodecoder)
target_link_libraries(${PROJECT_NAME} PRIVATE libaudiodecoder )
# lib-qt-qml-tricks
add_subdirectory(3rdparty/lib-qt-qml-tricks)
target_link_libraries(${PROJECT_NAME} PUBLIC libqtqmltricks)
# liblzr (optional)
if(LD_CORE_ENABLE_LIBLZR)
    add_subdirectory(3rdparty/liblzr)
    target_link_libraries(${PROJECT_NAME} PUBLIC liblzr)
endif()
# lua
add_subdirectory(3rdparty/lua)
target_link_libraries(${PROJECT_NAME} PRIVATE liblua)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/lua/src)
# nanosvg
# No need to do anything, header-only library
# opencv (optional)
if(LD_CORE_ENABLE_OPENCV)
    include(3rdparty/opencv/CMakeLists.txt)
    target_link_libraries(${PROJECT_NAME} PRIVATE opencv_core opencv_imgproc opencv_highgui opencv_videoio) # opencv_imgcodecs is dependency
endif()
# OpenLase
add_subdirectory(3rdparty/openlase)
target_link_libraries(${PROJECT_NAME} PUBLIC openlase)

# platform specific libraries
if(WIN32)
    if(LD_CORE_ENABLE_MIDI)
        target_link_libraries(${PROJECT_NAME} PRIVATE winmm.lib)
    endif()
endif()

if(APPLE)
    find_library(COREMIDI_LIBRARY CoreMIDI)
    mark_as_advanced(COREMIDI_LIBRARY)
    target_link_libraries(${PROJECT_NAME}  PRIVATE ${COREMIDI_LIBRARY})
endif()

if(ANDROID)
    find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS AndroidExtras REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::AndroidExtras)

    target_link_libraries(${PROJECT_NAME} PRIVATE GLESv2)
endif()


# add example for library. qt quick module is required
if(LD_CORE_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

# ----------------------------  tests ----------------------
if(LD_CORE_BUILD_TESTS)
    add_subdirectory(tests)
endif()

