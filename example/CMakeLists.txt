cmake_minimum_required(VERSION 3.5)

project(ldCore_Example)

set(CMAKE_VERBOSE_MAKEFILE ON)

if(WIN32)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

# setup C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ---------------------------- SOURCES  ----------------------

set(SOURCE_FILES
    src/main.cpp
    src/ldCoreExample.cpp
    src/ldCoreExample.h
    # src/Games
    src/Games/ldSpiralFighterGame.cpp
    src/Games/ldSpiralFighterGame.h
    #src/Visualizations/Visualizers
    src/Visualizations/Visualizers/Animation/ldLdva2AnimationVisualizer.cpp
    src/Visualizations/Visualizers/Animation/ldLdva2AnimationVisualizer.h
    src/Visualizations/Visualizers/Animation/ldGoGoGirlAnimationVisualizer.cpp
    src/Visualizations/Visualizers/Animation/ldGoGoGirlAnimationVisualizer.h
    src/Visualizations/Visualizers/Clock/ldClockComplexObject.cpp
    src/Visualizations/Visualizers/Clock/ldClockComplexObject.h
    src/Visualizations/Visualizers/Clock/ldAnalogClockVisualizer.cpp
    src/Visualizations/Visualizers/Clock/ldAnalogClockVisualizer.h
    src/Visualizations/Visualizers/Games/ldSpiralFighterVisualizer.cpp
    src/Visualizations/Visualizers/Games/ldSpiralFighterVisualizer.h
    src/Visualizations/Visualizers/Games/SpiralFighter/ldSpiralFighterEnemy.cpp
    src/Visualizations/Visualizers/Games/SpiralFighter/ldSpiralFighterEnemy.h
    src/Visualizations/Visualizers/Games/SpiralFighter/ldSpiralFighterMissile.cpp
    src/Visualizations/Visualizers/Games/SpiralFighter/ldSpiralFighterMissile.h
    src/Visualizations/Visualizers/Games/SpiralFighter/ldSpiralFighterPlayer.cpp
    src/Visualizations/Visualizers/Games/SpiralFighter/ldSpiralFighterPlayer.h
    src/Visualizations/Visualizers/Simple/ldCircleVisualizer.cpp
    src/Visualizations/Visualizers/Simple/ldCircleVisualizer.h
    src/Visualizations/Visualizers/Simple/ldSquareVisualizer.cpp
    src/Visualizations/Visualizers/Simple/ldSquareVisualizer.h
    src/Visualizations/Visualizers/Spectrum/ldSpectrumBandVisualizer.cpp
    src/Visualizations/Visualizers/Spectrum/ldSpectrumBandVisualizer.h
    )

list(APPEND SOURCE_FILES
    res/qml.qrc
    )


# Executable
if (ANDROID)
    add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32 ${SOURCE_FILES} )
endif()

# ---------------------------- DEPENDENCIES  ----------------------

# Qt
set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")
find_package(Qt5 CONFIG REQUIRED Core Gui Multimedia Qml)
list(APPEND LIBRARIES Qt5::Core  Qt5::Gui Qt5::Multimedia Qt5::Qml )


# ldCore
include_directories(../lib)
include_directories(../lib/3rdparty)
include_directories(../lib/3rdparty/lib-qt-qml-tricks/include)
list(APPEND LIBRARIES ldCore)

target_link_libraries(${PROJECT_NAME} ${LIBRARIES})


# ---------------------------- DEPLOY  ----------------------

# resources dir
set(SOURCE_RESOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resExt")

if(WIN32)
    # Copy required dlls to output folder
    if(CMAKE_GENERATOR STREQUAL "Visual Studio 14 2015")
        set(BUILD_SUB_PATH "${CMAKE_BUILD_TYPE}")
    else()
        set(BUILD_SUB_PATH "")
    endif()

    set(EXEDIR_PATH ${CMAKE_CURRENT_BINARY_DIR})

    set(LDCORELIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/ldCore.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LDCORELIB_PATH} ${EXEDIR_PATH})

    set(LIBUSB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../lib/3rdparty/laserdocklib/libusb/libusb-1.0.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LIBUSB_PATH} ${EXEDIR_PATH})

    set(LASERDOCKLIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/3rdparty/laserdocklib${BUILD_SUB_PATH}/laserdocklib.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LASERDOCKLIB_PATH} ${EXEDIR_PATH})

    set(OPENLASE_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/3rdparty/openlase${BUILD_SUB_PATH}/OpenLase.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OPENLASE_PATH} ${EXEDIR_PATH})

    if(LD_CORE_ENABLE_LIBLZR)
        set(LIBLZR_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/3rdparty/liblzr${BUILD_SUB_PATH}/liblzr.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LIBLZR_PATH} ${EXEDIR_PATH}
            )
    endif()

    if(LD_CORE_ENABLE_OPENCV)
        #opencv
        include(../lib/3rdparty/opencv/CMakeLists.txt)

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(DEBUG_SUFFIX "d")
        else()
            set(DEBUG_SUFFIX "")
        endif()

        set(OPENCVCORE_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_core320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVCORE_PATH} ${EXEDIR_PATH})
        set(OPENCVHIGHGUI_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_highgui320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVHIGHGUI_PATH} ${EXEDIR_PATH})
        set(OPENCVIMGCODECS_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_imgcodecs320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVIMGCODECS_PATH} ${EXEDIR_PATH})
        set(OPENCVIMGPROC_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_imgproc320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVIMGPROC_PATH} ${EXEDIR_PATH})
        set(OPENCVVIDEOIO_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_videoio320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVVIDEOIO_PATH} ${EXEDIR_PATH})
    endif()

    # copy resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_RESOURCES_PATH} "${EXEDIR_PATH}" || true) # "|| true" - trick to return always no error code
endif()

if(APPLE)
    # set correct bundle prefix for Xcode generator
    if(CMAKE_GENERATOR STREQUAL Xcode)
        set(BUNDLE_SUB_PATH "${CMAKE_CURRENT_BINARY_DIR}/example/\${CONFIGURATION}")
    else()
        set(BUNDLE_SUB_PATH ${CMAKE_CURRENT_BINARY_DIR}/example)
    endif()
    # set bundle pathes
    set(BUNDLE_PATH "${BUNDLE_SUB_PATH}/${PROJECT_NAME}.app")
    set(BUNDLE_RESOURCES_PATH "${BUNDLE_PATH}/Contents/Resources")

    # copy resourceas
    message(STATUS ${SOURCE_RESOURCES_PATH} ${BUNDLE_RESOURCES_PATH})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp -R -n ${SOURCE_RESOURCES_PATH} ${BUNDLE_RESOURCES_PATH} || true) # "|| true" - trick to return always no error code

endif()
