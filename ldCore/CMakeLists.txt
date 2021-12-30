cmake_minimum_required(VERSION 3.11)

# enable features
option(LD_CORE_ENABLE_MIDI "Enable midi" OFF)
option(LD_CORE_ENABLE_OPENCV "Enable opencv support" OFF)
option(LD_CORE_ENABLE_QT_QUICK "Enable Qt Quick module" OFF) # required for example
# some helpful flags
option(LD_CORE_GAMES_ALWAYS_PLAY_STATE "Games play after pause automatically" OFF)

include(CMakeDependentOption)
CMAKE_DEPENDENT_OPTION(LD_CORE_ENABLE_QT_QUICK "Enable Qt Quick module" OFF
    "NOT LD_CORE_BUILD_EXAMPLE" ON)

CMAKE_DEPENDENT_OPTION(LD_CORE_ENABLE_MIDI "Enable midi" OFF
    "NOT LD_ANDROID_BUILD" OFF)

project(ldCore)

if(LD_IOS_BUILD)
    add_library(${PROJECT_NAME})
else()
    add_library(${PROJECT_NAME} SHARED )
endif()

include(SetupCppFlags)
setup_cpp_flags(${PROJECT_NAME})

set_target_properties(ldCore PROPERTIES RESOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/resExt")

# setup library include dirs
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

# sources
set(INCDIR include/ldCore)
set(SRCDIR src)
target_sources(${PROJECT_NAME} PRIVATE
        ${INCDIR}/ldCore_global.h
        ${SRCDIR}/ldCore.cpp
        ${INCDIR}/ldCore.h
        #Helpers/
        ${INCDIR}/Helpers/ldEnumHelper.h
        ${SRCDIR}/Helpers/ldLaserController.cpp
        ${INCDIR}/Helpers/ldLaserController.h
        ${SRCDIR}/Helpers/ldPropertyObject.cpp
        ${INCDIR}/Helpers/ldPropertyObject.h
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
        ${SRCDIR}/Data/ldHardwareDataWorker.cpp
        ${INCDIR}/Data/ldHardwareDataWorker.h
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
        ${SRCDIR}/Filter/ldFilterProcessor.cpp
        ${INCDIR}/Filter/ldFilterProcessor.h
        ${SRCDIR}/Filter/ldHardwareFilter.cpp
        ${INCDIR}/Filter/ldHardwareFilter.h
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
        ${SRCDIR}/Hardware/ldNetworkHardware.cpp
        ${INCDIR}/Hardware/ldNetworkHardware.h
        ${SRCDIR}/Hardware/ldNetworkHardwareManager.cpp
        ${INCDIR}/Hardware/ldNetworkHardwareManager.h
        #Render
        ${SRCDIR}/Render/ldAbstractRenderer.cpp
        ${INCDIR}/Render/ldAbstractRenderer.h
        ${SRCDIR}/Render/ldRendererOpenlase.cpp
        ${INCDIR}/Render/ldRendererOpenlase.h
        #Settings
        ${SRCDIR}/Settings/ldSettings.cpp
        ${INCDIR}/Settings/ldSettings.h
        #Simulator
        ${SRCDIR}/Simulator/ldSimulatorEngine.cpp
        ${INCDIR}/Simulator/ldSimulatorEngine.h
        ${SRCDIR}/Simulator/ldSimulatorGrid.cpp
        ${INCDIR}/Simulator/ldSimulatorGrid.h
        ${SRCDIR}/Simulator/ldSimulatorProcessor.cpp
        ${SRCDIR}/Simulator/ldSimulatorProcessor.h
        ${SRCDIR}/Simulator/ldSimulatorRenderer.cpp
        ${INCDIR}/Simulator/ldSimulatorRenderer.h
        #Shape
        ${SRCDIR}/Shape/ldList.cpp
        ${INCDIR}/Shape/ldList.h
        ${SRCDIR}/Shape/ldParticleGeometry.cpp
        ${INCDIR}/Shape/ldParticleGeometry.h
        ${INCDIR}/Shape/ldShader.h
        ${SRCDIR}/Shape/ldShaderManager.cpp
        ${INCDIR}/Shape/ldShaderManager.h
        ${SRCDIR}/Shape/ldShape.cpp
        ${INCDIR}/Shape/ldShape.h
        #Sound
        ${SRCDIR}/Sound/ldAudioDecoder.cpp
        ${INCDIR}/Sound/ldAudioDecoder.h
        ${SRCDIR}/Sound/ldAudioFileDecoder.cpp
        ${INCDIR}/Sound/ldAudioFileDecoder.h
        ${SRCDIR}/Sound/ldFFT.cpp
        ${SRCDIR}/Sound/ldFFT.h
        ${SRCDIR}/Sound/ldQAudioInputDevice.cpp
        ${SRCDIR}/Sound/ldQAudioInputDevice.h
        ${SRCDIR}/Sound/ldSoundAnalyzer.cpp
        ${INCDIR}/Sound/ldSoundAnalyzer.h
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
        ${INCDIR}/Utilities/ldAudioBuffer.h
        ${INCDIR}/Utilities/ldBuffer.h
        ${INCDIR}/Utilities/ldCircularBuffer.h
        ${INCDIR}/Utilities/ldCompressedSample.h
        ${SRCDIR}/Utilities/ldCompressedSample.cpp
        ${INCDIR}/Utilities/ldThreadSafeQueue.h
        ${INCDIR}/Utilities/ldUtils.h
        ${INCDIR}/Utilities/ldVertex.h
        ${SRCDIR}/Utilities/ldVertex.cpp
        ${INCDIR}/Utilities/ldVertexFrame.h
        ${SRCDIR}/Utilities/ldVertexFrame.cpp
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
        ${SRCDIR}/Helpers/Audio/ldBeatDetector.cpp
        ${INCDIR}/Helpers/Audio/ldBeatDetector.h
        ${SRCDIR}/Helpers/Audio/ldMultipleBeatDetector.cpp
        ${INCDIR}/Helpers/Audio/ldMultipleBeatDetector.h
        ${SRCDIR}/Helpers/Audio/ldBpmBeatDetector.cpp
        ${INCDIR}/Helpers/Audio/ldBpmBeatDetector.h
        ${SRCDIR}/Helpers/Audio/ldDurationalStatEstimator.cpp
        ${INCDIR}/Helpers/Audio/ldDurationalStatEstimator.h
        ${SRCDIR}/Helpers/Audio/ldHybridReactor.cpp
        ${INCDIR}/Helpers/Audio/ldHybridReactor.h
        ${SRCDIR}/Helpers/Audio/ldManualBpm.cpp
        ${INCDIR}/Helpers/Audio/ldManualBpm.h
        ${SRCDIR}/Helpers/Audio/ldManualBpmBeat.cpp
        ${INCDIR}/Helpers/Audio/ldManualBpmBeat.h
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
        ${INCDIR}/Helpers/Maths/ldAnimatedAngle.cpp
        ${INCDIR}/Helpers/Maths/ldAnimatedAngle.h
        ${SRCDIR}/Helpers/Maths/ldFilterMath.cpp
        ${INCDIR}/Helpers/Maths/ldFilterMath.h
        ${SRCDIR}/Helpers/Maths/ldGeometryUtil.cpp
        ${INCDIR}/Helpers/Maths/ldGeometryUtil.h
        ${INCDIR}/Helpers/Maths/ldGlobals.h
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
        ${SRCDIR}/Helpers/Maths/ldSmoothUtil.cpp
        ${INCDIR}/Helpers/Maths/ldSmoothUtil.h
        ${SRCDIR}/Helpers/Maths/ldVec2.cpp
        ${INCDIR}/Helpers/Maths/ldVec2.h
        ${SRCDIR}/Helpers/Maths/ldVec3.cpp
        ${INCDIR}/Helpers/Maths/ldVec3.h
        #Helpers/SimpleCrypt
        ${SRCDIR}/Helpers/SimpleCrypt/ldSimpleCrypt.cpp
        ${INCDIR}/Helpers/SimpleCrypt/ldSimpleCrypt.h
        #Helpers/Sound
        ${SRCDIR}/Helpers/Sound/ldQSound.cpp
        ${INCDIR}/Helpers/Sound/ldQSound.h
        ${SRCDIR}/Helpers/Sound/ldSoundEffects.cpp
        ${INCDIR}/Helpers/Sound/ldSoundEffects.h
        #Helpers/SVG
        ${SRCDIR}/Helpers/SVG/ldSvgDir.cpp
        ${INCDIR}/Helpers/SVG/ldSvgDir.h
        ${SRCDIR}/Helpers/SVG/ldSvgReader.cpp
        ${INCDIR}/Helpers/SVG/ldSvgReader.h
        #Helpers/Text
        ${SRCDIR}/Helpers/Text/ldAbstractText.cpp
        ${INCDIR}/Helpers/Text/ldAbstractText.h
        ${SRCDIR}/Helpers/Text/ld3dTextLabelOne.cpp
        ${INCDIR}/Helpers/Text/ld3dTextLabelOne.h
        ${SRCDIR}/Helpers/Text/ldSvgFont.cpp
        ${INCDIR}/Helpers/Text/ldSvgFont.h
        ${SRCDIR}/Helpers/Text/ldSvgFontManager.cpp
        ${INCDIR}/Helpers/Text/ldSvgFontManager.h
        ${SRCDIR}/Helpers/Text/ldSvgLetter.cpp
        ${INCDIR}/Helpers/Text/ldSvgLetter.h
        ${SRCDIR}/Helpers/Text/ldTextCounterOne.cpp
        ${INCDIR}/Helpers/Text/ldTextCounterOne.h
        ${SRCDIR}/Helpers/Text/ldTextLabel.cpp
        ${INCDIR}/Helpers/Text/ldTextLabel.h
        ${SRCDIR}/Helpers/Text/ldTextSvgHelper.cpp
        ${INCDIR}/Helpers/Text/ldTextSvgHelper.h
        ${SRCDIR}/Helpers/Text/ldTicker.cpp
        ${INCDIR}/Helpers/Text/ldTicker.h
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

# loopback audio support on Windows
if(WIN32 OR LD_ANDROID_BUILD)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_LOOPBACK_DEVICE_ENABLED)

    if(WIN32)
        target_sources(${PROJECT_NAME} PRIVATE
            ${SRCDIR}/Sound/win/ldLoopbackAudioDevice.cpp
            ${SRCDIR}/Sound/win/ldLoopbackAudioDevice.h
            ${SRCDIR}/Sound/win/ldLoopbackAudioDeviceWorker.cpp
            ${SRCDIR}/Sound/win/ldLoopbackAudioDeviceWorker.h
            )
    endif()

    if(LD_ANDROID_BUILD)
        target_sources(${PROJECT_NAME} PRIVATE
            ${SRCDIR}/Sound/android/ldLoopbackAudioDevice.cpp
            ${SRCDIR}/Sound/android/ldLoopbackAudioDevice.h
            )
    endif()

endif()


if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LDCORE_LIBRARY)

    target_compile_definitions(${PROJECT_NAME} PUBLIC _USE_MATH_DEFINES)
endif()

if(LD_BUILD_RELEASE_VERSION)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LD_BUILD_RELEASE_VERSION)
endif()

if(LD_CORE_GAMES_ALWAYS_PLAY_STATE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_CORE_GAMES_ALWAYS_PLAY_STATE)
endif()


# -------------  Qt -----------

set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")

set(QT5_MIN_VERSION 5.14)
find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS Core Gui Multimedia Gamepad REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core Qt5::Gui Qt5::Multimedia Qt5::Gamepad)


# -------------  3RDPARTY -----------

# public
target_link_libraries(${PROJECT_NAME} PUBLIC
    3rdparty::aubio
    3rdparty::laserdocklib
    3rdparty::libqtqmltricks
    3rdparty::openlase
    )

# private
target_link_libraries(${PROJECT_NAME} PRIVATE
    3rdparty::kiss_fft130
    3rdparty::libaudiodecoder
    3rdparty::simplecrypt
    3rdparty::nanosvg
    )

if(LASERDOCKLIB_USB_SUPPORT)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRCDIR}/Hardware/ldUSBHardware.cpp
        ${INCDIR}/Hardware/ldUSBHardware.h
        ${SRCDIR}/Hardware/ldUSBHardwareManager.cpp
        ${INCDIR}/Hardware/ldUSBHardwareManager.h
        )

 endif()

# ------------  LD_CORE_ENABLE_LIBLZR -------------

if(LD_CORE_ENABLE_LIBLZR)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRCDIR}/Helpers/ldLzrHelper.cpp
        ${INCDIR}/Helpers/ldLzrHelper.h
        )

    target_link_libraries(${PROJECT_NAME} PUBLIC 3rdparty::liblzr)
endif()

# -------------  LD_CORE_ENABLE_MIDI ----------

if(LD_CORE_ENABLE_MIDI)
    target_compile_definitions(${PROJECT_NAME} PUBLIC LD_CORE_ENABLE_MIDI)

    target_sources(${PROJECT_NAME} PRIVATE
        #Sound
        ${SRCDIR}/Sound/ldMidiDevice.cpp
        ${SRCDIR}/Sound/ldMidiDevice.h
        ${SRCDIR}/Sound/Midi/ldMidiInfo.cpp
        ${INCDIR}/Sound/Midi/ldMidiInfo.h
        ${SRCDIR}/Sound/Midi/ldMidiInput.cpp
        ${INCDIR}/Sound/Midi/ldMidiInput.h
        )

    if(WIN32)
        target_sources(${PROJECT_NAME} PRIVATE
            ${SRCDIR}/Sound/Midi/ldMidiInput_win.cpp
            )

        target_link_libraries(${PROJECT_NAME} PRIVATE winmm.lib)
    elseif(APPLE)
        target_sources(${PROJECT_NAME} PRIVATE
            ${SRCDIR}/Sound/Midi/ldMidiInput_mac.mm
            )

        find_library(COREMIDI_LIBRARY CoreMIDI)
        mark_as_advanced(COREMIDI_LIBRARY)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${COREMIDI_LIBRARY})
    else()
        # just stub to compile
        message(WARNING "ldMidiInput stub is used, midi is not implemented on platform")
        target_sources(${PROJECT_NAME} PRIVATE
            ${SRCDIR}/Sound/Midi/ldMidiInput_stub.cpp
            )
    endif()
endif()

# -----------  LD_CORE_ENABLE_OPENCV -----------

if(LD_CORE_ENABLE_OPENCV)
    target_sources(${PROJECT_NAME} PRIVATE
        #Helpers/Image
        ${SRCDIR}/Helpers/Image/ldImageHelper.cpp
        ${INCDIR}/Helpers/Image/ldImageHelper.h
        )


    # lib
    include(${CMAKE_CURRENT_LIST_DIR}/../3rdparty/opencv/CMakeLists.txt)
    if(LD_IOS_BUILD)
        target_include_directories(${PROJECT_NAME} PUBLIC ${OPENCV_FRAMEWORK}/Headers)
        target_link_libraries(${PROJECT_NAME}  PUBLIC ${OPENCV_FRAMEWORK}/opencv2)
    else()
        target_link_libraries(${PROJECT_NAME} PUBLIC opencv_core opencv_imgcodecs opencv_imgproc opencv_highgui opencv_videoio opencv_features2d opencv_calib3d )
    endif()
endif()

# ------------  LD_CORE_ENABLE_OPENMP -------------

if(LD_CORE_ENABLE_OPENMP)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        3rdparty::openmp
        )
endif()

# -----------  LD_CORE_ENABLE_QUAZIP -----------

if(LD_CORE_ENABLE_QUAZIP)
    target_sources(${PROJECT_NAME} PRIVATE
        # TODO: move
        ${SRCDIR}/Android/ldZipExtractor.cpp
        ${INCDIR}/Android/ldZipExtractor.h
        )

    target_link_libraries(${PROJECT_NAME} PRIVATE 3rdparty::quazip)
endif()

# ------------  LD_CORE_ENABLE_QT_QUICK ------------

if(LD_CORE_ENABLE_QT_QUICK)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRCDIR}/Simulator/ldSimulatorItem.cpp
        ${INCDIR}/Simulator/ldSimulatorItem.h
        )

    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_CORE_ENABLE_QT_QUICK)

    find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS Quick REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Quick)
endif()

# ------------  LD_CORE_RESOURCES_EXTRACTOR -----------

if(LD_CORE_RESOURCES_EXTRACTOR)
    target_sources(${PROJECT_NAME} PRIVATE
        ${SRCDIR}/Android/ldResourcesExtractor.cpp
        ${INCDIR}/Android/ldResourcesExtractor.h
        )

    target_compile_definitions(${PROJECT_NAME} PUBLIC LD_CORE_RESOURCES_EXTRACTOR)
endif()

# -------------  Android ----------

if(ANDROID)
    find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS AndroidExtras REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::AndroidExtras)

    target_link_libraries(${PROJECT_NAME} PRIVATE GLESv2)
endif()

if(LD_IOS_BUILD)
    target_link_libraries(${PROJECT_NAME} PRIVATE z)
endif()



