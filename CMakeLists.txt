cmake_minimum_required(VERSION 3.11)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

# main app configuration
option(LD_ANDROID_BUILD "Build for Android" OFF)
option(LD_CORE_BUILD_EXAMPLE "Build Example" OFF)
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
set(CMAKE_CXX_STANDARD 11)
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
        ${SRCDIR}/Filter/ldColorUtils.cpp
        ${INCDIR}/Filter/ldColorUtils.h
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
        ${SRCDIR}/Sound/ldAudioDecoder.h
        ${SRCDIR}/Sound/ldFFT.cpp
        ${SRCDIR}/Sound/ldFFT.h
        ${SRCDIR}/Sound/ldQAudioDecoder.cpp
        ${SRCDIR}/Sound/ldQAudioDecoder.h
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
        #Visualizations/util/AudioHelper
        ${SRCDIR}/Visualizations/util/AudioHelper/ldAppakaBeat.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldAppakaBeat.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldAppakBpmSelector.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldAppakBpmSelector.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldAppakGate.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldAppakGate.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldAppakPeaks.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldAppakPeaks.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldAppakSpectrum.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldAppakSpectrum.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldAudioBasic.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldAudioBasic.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldDurationalStatEstimator.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldDurationalStatEstimator.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldHybridReactor.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldHybridReactor.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldMusicFeature.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldMusicFeature.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldNoiseField.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldNoiseField.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldOnsetDetector.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldOnsetDetector.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldPitchDetector.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldPitchDetector.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldSilentThree.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldSilentThree.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldSoundGate.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldSoundGate.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldSoundEventUtil.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldSoundEventUtil.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldSpectrogram.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldSpectrogram.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldSpectrumFrame.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldSpectrumFrame.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldTempoAC.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldTempoAC.h
        ${SRCDIR}/Visualizations/util/AudioHelper/ldTempoTracker.cpp
        ${INCDIR}/Visualizations/util/AudioHelper/ldTempoTracker.h
        #Visualizations/util/BezierCurveHelper
        ${SRCDIR}/Visualizations/util/BezierCurveHelper/ld3dBezierCurveFrame.cpp
        ${INCDIR}/Visualizations/util/BezierCurveHelper/ld3dBezierCurveFrame.h
        ${SRCDIR}/Visualizations/util/BezierCurveHelper/ld3dBezierCurveObject.cpp
        ${INCDIR}/Visualizations/util/BezierCurveHelper/ld3dBezierCurveObject.h
        ${SRCDIR}/Visualizations/util/BezierCurveHelper/ldBezierCurveFrame.cpp
        ${INCDIR}/Visualizations/util/BezierCurveHelper/ldBezierCurveFrame.h
        ${SRCDIR}/Visualizations/util/BezierCurveHelper/ldBezierCurveObject.cpp
        ${INCDIR}/Visualizations/util/BezierCurveHelper/ldBezierCurveObject.h
        #Visualizations/util/ColorHelper
        ${SRCDIR}/Visualizations/util/ColorHelper/ldColorUtil.cpp
        ${INCDIR}/Visualizations/util/ColorHelper/ldColorUtil.h
        ${SRCDIR}/Visualizations/util/ColorHelper/ldColorUtilMEO.cpp
        ${INCDIR}/Visualizations/util/ColorHelper/ldColorUtilMEO.h
        #Visualizations/util/DrawHelper
        ${SRCDIR}/Visualizations/util/DrawHelper/ld3dBezierCurveDrawer.cpp
        ${INCDIR}/Visualizations/util/DrawHelper/ld3dBezierCurveDrawer.h
        ${SRCDIR}/Visualizations/util/DrawHelper/ldBezierCurveDrawer.cpp
        ${INCDIR}/Visualizations/util/DrawHelper/ldBezierCurveDrawer.h
        #Visualizations/util/MathsHelper
        ${SRCDIR}/Visualizations/util/MathsHelper/ldGeometryUtil.cpp
        ${INCDIR}/Visualizations/util/MathsHelper/ldGeometryUtil.h
        ${SRCDIR}/Visualizations/util/MathsHelper/ldMathSpectrum.cpp
        ${INCDIR}/Visualizations/util/MathsHelper/ldMathSpectrum.h
        ${SRCDIR}/Visualizations/util/MathsHelper/ldMathStat.cpp
        ${INCDIR}/Visualizations/util/MathsHelper/ldMathStat.h
        ${SRCDIR}/Visualizations/util/MathsHelper/ldMaths.cpp
        ${INCDIR}/Visualizations/util/MathsHelper/ldMaths.h
        ${SRCDIR}/Visualizations/util/MathsHelper/ldSimpleVector.cpp
        ${INCDIR}/Visualizations/util/MathsHelper/ldSimpleVector.h
        ${SRCDIR}/Visualizations/util/MathsHelper/ldSmoothUtil.cpp
        ${INCDIR}/Visualizations/util/MathsHelper/ldSmoothUtil.h
        #Visualizations/util/SoundHelper
        ${SRCDIR}/Visualizations/util/SoundHelper/ldQSound.cpp
        ${INCDIR}/Visualizations/util/SoundHelper/ldQSound.h
        ${SRCDIR}/Visualizations/util/SoundHelper/ldSoundEffects.cpp
        ${INCDIR}/Visualizations/util/SoundHelper/ldSoundEffects.h
        #Visualizations/util/SVGHelper
        ${SRCDIR}/Visualizations/util/SVGHelper/ldSvgDir.cpp
        ${INCDIR}/Visualizations/util/SVGHelper/ldSvgDir.h
        ${SRCDIR}/Visualizations/util/SVGHelper/ldSvgObject.cpp
        ${INCDIR}/Visualizations/util/SVGHelper/ldSvgObject.h
        ${SRCDIR}/Visualizations/util/SVGHelper/ldSvgReader.cpp
        ${INCDIR}/Visualizations/util/SVGHelper/ldSvgReader.h
        #Visualizations/util/TextHelper
        ${SRCDIR}/Visualizations/util/TextHelper/ldAbstractText.cpp
        ${INCDIR}/Visualizations/util/TextHelper/ldAbstractText.h
        ${SRCDIR}/Visualizations/util/TextHelper/ld3dTextLabelOne.cpp
        ${INCDIR}/Visualizations/util/TextHelper/ld3dTextLabelOne.h
        ${SRCDIR}/Visualizations/util/TextHelper/ldFont.cpp
        ${INCDIR}/Visualizations/util/TextHelper/ldFont.h
        ${SRCDIR}/Visualizations/util/TextHelper/ldSvgLetter.cpp
        ${INCDIR}/Visualizations/util/TextHelper/ldSvgLetter.h
        ${SRCDIR}/Visualizations/util/TextHelper/ldTextCounterOne.cpp
        ${INCDIR}/Visualizations/util/TextHelper/ldTextCounterOne.h
        ${SRCDIR}/Visualizations/util/TextHelper/ldTextLabel.cpp
        ${INCDIR}/Visualizations/util/TextHelper/ldTextLabel.h
        ${SRCDIR}/Visualizations/util/TextHelper/ldTextSvgHelper.cpp
        ${INCDIR}/Visualizations/util/TextHelper/ldTextSvgHelper.h
        #Visualizations/util/VisualizerHelper
        ${SRCDIR}/Visualizations/util/VisualizerHelper/ldAnimTimerUtil.cpp
        ${INCDIR}/Visualizations/util/VisualizerHelper/ldAnimTimerUtil.h
        ${SRCDIR}/Visualizations/util/VisualizerHelper/ldAnimationSequence.cpp
        ${INCDIR}/Visualizations/util/VisualizerHelper/ldAnimationSequence.h
        ${SRCDIR}/Visualizations/util/VisualizerHelper/ldBeatTracker.cpp
        ${INCDIR}/Visualizations/util/VisualizerHelper/ldBeatTracker.h
        ${SRCDIR}/Visualizations/util/VisualizerHelper/ldLaserFilter.cpp
        ${INCDIR}/Visualizations/util/VisualizerHelper/ldLaserFilter.h
        ${SRCDIR}/Visualizations/util/VisualizerHelper/ldLineGraph.cpp
        ${INCDIR}/Visualizations/util/VisualizerHelper/ldLineGraph.h
        ${SRCDIR}/Visualizations/util/VisualizerHelper/ldVisualizerHelper.cpp
        ${INCDIR}/Visualizations/util/VisualizerHelper/ldVisualizerHelper.h
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
        #Visualizations/util/ImageHelper
        ${SRCDIR}/Visualizations/util/ImageHelper/ldImageHelper.cpp
        ${INCDIR}/Visualizations/util/ImageHelper/ldImageHelper.h
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
    target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)
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
    target_link_libraries(${PROJECT_NAME} PRIVATE liblzr)
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

