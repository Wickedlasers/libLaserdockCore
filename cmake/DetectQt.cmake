
# firstly check environmental variable
if((EXISTS $ENV{QTDIR}) AND (NOT DEFINED QTDIR))
    set(QTDIR $ENV{QTDIR} CACHE STRING "Path to Qt dir")
endif()

# if it is not available try to define from QT_BASE_DIR
if(NOT DEFINED QTDIR)
    if(NOT DEFINED QT_BASE_DIR)
        if(APPLE)
            set(QT_BASE_DIR "/Users/ncuxer/Qt/6.7.3")
        endif()
    endif()
    if(LD_ANDROID_BUILD )
        set(QTDIR "${QT_BASE_DIR}/android")
    else()
        # win
        if(WIN32)
            set(QTDIR "${QT_BASE_DIR}/msvc2017")
        endif()
        # mac
        if(APPLE)
            if(LD_IOS_BUILD)
                set(QTDIR "${QT_BASE_DIR}/ios")
            else()
                set(QTDIR "${QT_BASE_DIR}/macos")
            endif()
        endif()
        # mac
        if(LINUX)
            set(QTDIR "${QT_BASE_DIR}/gcc_64")
        endif()
    endif()
endif()

message(STATUS "QTDIR: ${QTDIR}")

list(APPEND CMAKE_PREFIX_PATH ${QTDIR})
list(APPEND CMAKE_FIND_ROOT_PATH ${QTDIR})

# detect if qt6 or 5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

message(STATUS "QT_VERSION_MAJOR: ${QT_VERSION_MAJOR}")


# For some reason android Qt toolchain can't find cmake packages without direct
set(Qt${QT_VERSION_MAJOR}_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}")
if(QT_VERSION_MAJOR EQUAL 5)
    set(Qt${QT_VERSION_MAJOR}AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}AndroidExtras")
endif()
set(Qt${QT_VERSION_MAJOR}Core_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Core")
set(Qt${QT_VERSION_MAJOR}GamepadLegacy_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}GamepadLegacy")
set(Qt${QT_VERSION_MAJOR}Gamepad_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Gamepad")
set(Qt${QT_VERSION_MAJOR}Gui_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Gui")
set(Qt${QT_VERSION_MAJOR}Multimedia_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Multimedia")
set(Qt${QT_VERSION_MAJOR}Network_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Network")
set(Qt${QT_VERSION_MAJOR}OpenGL_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}OpenGL")
set(Qt${QT_VERSION_MAJOR}Positioning_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Positioning")
set(Qt${QT_VERSION_MAJOR}Qml_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Qml")
set(Qt${QT_VERSION_MAJOR}QmlModels_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}QmlModels")
set(Qt${QT_VERSION_MAJOR}Quick_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Quick")
set(Qt${QT_VERSION_MAJOR}QuickConfig_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}QuickConfig")
set(Qt${QT_VERSION_MAJOR}Test_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Test")
set(Qt${QT_VERSION_MAJOR}Sensors_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Sensors")
set(Qt${QT_VERSION_MAJOR}Svg_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Svg")
set(Qt${QT_VERSION_MAJOR}Widgets_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Widgets")
