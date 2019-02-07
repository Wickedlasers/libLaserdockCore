cmake_minimum_required(VERSION 3.11)

project(ldCore_Example)

set(CMAKE_VERBOSE_MAKEFILE ON)

# setup C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# setup Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ---------------------------- SOURCES  ----------------------

# Executable
if (ANDROID)
    add_library(${PROJECT_NAME} SHARED )
else()
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE WIN32)
endif()

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/ldCoreExample.cpp
    src/ldCoreExample.h
    #src/Visualizations/Visualizers
    src/Visualizations/Visualizers/Animation/ldAnimationVisualizer.cpp
    src/Visualizations/Visualizers/Animation/ldAnimationVisualizer.h
    src/Visualizations/Visualizers/Animation/ldGoGoGirlAnimationVisualizer.cpp
    src/Visualizations/Visualizers/Animation/ldGoGoGirlAnimationVisualizer.h
    src/Visualizations/Visualizers/Clock/ldClockComplexObject.cpp
    src/Visualizations/Visualizers/Clock/ldClockComplexObject.h
    src/Visualizations/Visualizers/Clock/ldAnalogClockVisualizer.cpp
    src/Visualizations/Visualizers/Clock/ldAnalogClockVisualizer.h
    src/Visualizations/Visualizers/Simple/ldCircleVisualizer.cpp
    src/Visualizations/Visualizers/Simple/ldCircleVisualizer.h
    src/Visualizations/Visualizers/Simple/ldSquareVisualizer.cpp
    src/Visualizations/Visualizers/Simple/ldSquareVisualizer.h
    src/Visualizations/Visualizers/Spectrum/ldSpectrumBandVisualizer.cpp
    src/Visualizations/Visualizers/Spectrum/ldSpectrumBandVisualizer.h
    )

target_sources(${PROJECT_NAME} PRIVATE
    res/qml.qrc
    )


# ---------------------------- DEPENDENCIES  ----------------------

# Qt
set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")
find_package(Qt5 CONFIG REQUIRED Core Gui Multimedia Qml)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core  Qt5::Gui Qt5::Multimedia Qt5::Qml)


# ldCore
target_include_directories(${PROJECT_NAME} PRIVATE ../3rdparty)
target_include_directories(${PROJECT_NAME} PRIVATE ../3rdparty/lib-qt-qml-tricks/include)

target_link_libraries(${PROJECT_NAME} PRIVATE ldCore)


add_subdirectory(SpiralFighter)
target_link_libraries(${PROJECT_NAME} PRIVATE ldSpiralFighter)

# ---------------------------- DEPLOY  ----------------------

# resources dir
set(SOURCE_RESOURCES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resExt")

if(WIN32)
    # Copy required dlls to output folder
    if(CMAKE_GENERATOR STREQUAL "Visual Studio 14 2015")
        set(BUILD_SUB_PATH "${CMAKE_BUILD_TYPE}")
    elseif(CMAKE_GENERATOR STREQUAL "Visual Studio 15 2017")
        set(BUILD_SUB_PATH "${CMAKE_CFG_INTDIR}")
    else()
        set(BUILD_SUB_PATH "")
    endif()

    set(EXEDIR_PATH ${CMAKE_CURRENT_BINARY_DIR})

    set(LDCORELIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../${BUILD_SUB_PATH}/ldCore.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LDCORELIB_PATH} ${EXEDIR_PATH})

    set(LIBUSB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/laserdocklib/3rdparty/libusb/lib/win/libusb-1.0.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LIBUSB_PATH} ${EXEDIR_PATH})

    set(LASERDOCKLIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/laserdocklib/${BUILD_SUB_PATH}/laserdocklib.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LASERDOCKLIB_PATH} ${EXEDIR_PATH})

    set(OPENLASE_PATH "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/openlase/${BUILD_SUB_PATH}/OpenLase.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OPENLASE_PATH} ${EXEDIR_PATH})

    if(LD_CORE_ENABLE_LIBLZR)
        set(LIBLZR_PATH "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/liblzr/${BUILD_SUB_PATH}/liblzr.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LIBLZR_PATH} ${EXEDIR_PATH}
            )
    endif()

    if(LD_CORE_ENABLE_OPENCV)
        #opencv
        include(../3rdparty/opencv/CMakeLists.txt)

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
        set(BUNDLE_SUB_PATH "${CMAKE_CURRENT_BINARY_DIR}/\${CONFIGURATION}")
    else()
        set(BUNDLE_SUB_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    # set bundle pathes
    set(BUNDLE_PATH "${BUNDLE_SUB_PATH}/${PROJECT_NAME}.app")
    set(BUNDLE_RESOURCES_PATH "${BUNDLE_PATH}/Contents/Resources")

    # copy resourceas
    message(STATUS ${SOURCE_RESOURCES_PATH} ${BUNDLE_RESOURCES_PATH})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp -R -n ${SOURCE_RESOURCES_PATH} ${BUNDLE_RESOURCES_PATH} || true) # "|| true" - trick to return always no error code

endif()
