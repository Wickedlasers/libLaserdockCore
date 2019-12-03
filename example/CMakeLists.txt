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

set_target_properties(${PROJECT_NAME} PROPERTIES RESOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/resExt")

if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _USE_MATH_DEFINES)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_sources(${PROJECT_NAME} PRIVATE
    src/main.cpp
    src/ldCoreExample.cpp
    src/ldCoreExample.h
    #src/Visualizations/Visualizers
    src/Visualizations/Ctrl/ldVisDirectionCtrl.cpp
    src/Visualizations/Ctrl/ldVisDirectionCtrl.h
    src/Visualizations/Visualizers/Animation/ldAnimationVisualizer.cpp
    src/Visualizations/Visualizers/Animation/ldAnimationVisualizer.h
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

if(ANDROID)
    set(Qt5AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt5AndroidExtras")
    find_package(Qt5 ${QT5_MIN_VERSION} COMPONENTS AndroidExtras REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::AndroidExtras)
endif()


# ldCore
target_include_directories(${PROJECT_NAME} PRIVATE ../3rdparty)
target_include_directories(${PROJECT_NAME} PRIVATE ../3rdparty/lib-qt-qml-tricks/include)

target_link_libraries(${PROJECT_NAME} PRIVATE ldCore)


add_subdirectory(SpiralFighter)
target_link_libraries(${PROJECT_NAME} PRIVATE ldSpiralFighter)

if(ANDROID)
    set(LD_EXAMPLE_PACKAGE_NAME "com.wickedlasers.example")
    set(LD_EXAMPLE_RESOURCES_VERSION_CODE 2)

    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_EXAMPLE_PACKAGE_NAME=\"${LD_EXAMPLE_PACKAGE_NAME}\")
    target_compile_definitions(${PROJECT_NAME} PRIVATE LD_EXAMPLE_RESOURCES_VERSION_CODE=${LD_EXAMPLE_RESOURCES_VERSION_CODE})
endif()

# ---------------------------- DEPLOY  ----------------------

# resources dir
get_target_property(SOURCE_RESOURCES_PATH ${PROJECT_NAME} RESOURCE_DIR)
get_target_property(SOURCE_LDCORE_RESOURCES_PATH ldCore RESOURCE_DIR)

if(WIN32)
    # Copy required dlls to output folder
    if(CMAKE_GENERATOR STREQUAL "Visual Studio 14 2015")
        set(BUILD_SUB_PATH "${CMAKE_BUILD_TYPE}")
    elseif(CMAKE_GENERATOR STREQUAL "Visual Studio 15 2017")
        set(BUILD_SUB_PATH "${CMAKE_CFG_INTDIR}")
    else()
        set(BUILD_SUB_PATH "")
    endif()

    set(EXEDIR_PATH ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_SUB_PATH})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ldCore> ${EXEDIR_PATH})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:libusb> ${EXEDIR_PATH})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:laserdocklib> ${EXEDIR_PATH})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:openlase> ${EXEDIR_PATH})

    if(TARGET liblzr)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:liblzr>  ${EXEDIR_PATH})
    endif()

    if(LD_CORE_ENABLE_OPENCV)
        #opencv
        include(../3rdparty/opencv/CMakeLists.txt)

        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:opencv_core> ${EXEDIR_PATH})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:opencv_highgui> ${EXEDIR_PATH})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:opencv_imgcodecs> ${EXEDIR_PATH})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:opencv_imgproc> ${EXEDIR_PATH})
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:opencv_videoio> ${EXEDIR_PATH})
    endif()

    # copy resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_RESOURCES_PATH} ${SOURCE_LDCORE_RESOURCES_PATH} "${EXEDIR_PATH}/Resources")
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

    # copy resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_RESOURCES_PATH} ${SOURCE_LDCORE_RESOURCES_PATH} ${BUNDLE_RESOURCES_PATH})
endif()

if (ANDROID)
    # create apk for android
    include(install/android/ldAndroidApk.cmake)
endif()


# debug environment for windows
if(CMAKE_GENERATOR STREQUAL "Visual Studio 14 2015" OR CMAKE_GENERATOR STREQUAL "Visual Studio 15 2017")
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
    configure_file(project.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.vcxproj.user @ONLY)
endif()

