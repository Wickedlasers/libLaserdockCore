
# firstly check environmental variable
if((EXISTS $ENV{QTDIR}) AND (NOT DEFINED QTDIR))
    set(QTDIR $ENV{QTDIR} CACHE STRING "Path to Qt dir")
endif()

# if it is not available try to define from QT_BASE_DIR
if(DEFINED QT_BASE_DIR AND NOT DEFINED QTDIR)
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
                set(QTDIR "${QT_BASE_DIR}/clang_64")
            endif()
        endif()
        # mac
        if(LINUX)
            set(QTDIR "${QT_BASE_DIR}/gcc_64")
        endif()
    endif()
endif()

message(STATUS "QTDIR: ${QTDIR}")

# For some reason android Qt toolchain can't find cmake packages without direct
set(Qt5_DIR "${QTDIR}/lib/cmake/Qt5")
set(Qt5AndroidExtras_DIR "${QTDIR}/lib/cmake/Qt5AndroidExtras")
set(Qt5Core_DIR "${QTDIR}/lib/cmake/Qt5Core")
set(Qt5Gamepad_DIR "${QTDIR}/lib/cmake/Qt5Gamepad")
set(Qt5Gui_DIR "${QTDIR}/lib/cmake/Qt5Gui")
set(Qt5Multimedia_DIR "${QTDIR}/lib/cmake/Qt5Multimedia")
set(Qt5Network_DIR "${QTDIR}/lib/cmake/Qt5Network")
set(Qt5OpenGL_DIR "${QTDIR}/lib/cmake/Qt5OpenGL")
set(Qt5Positioning_DIR "${QTDIR}/lib/cmake/Qt5Positioning")
set(Qt5Qml_DIR "${QTDIR}/lib/cmake/Qt5Qml")
set(Qt5QmlModels_DIR "${QTDIR}/lib/cmake/Qt5QmlModels")
set(Qt5Quick_DIR "${QTDIR}/lib/cmake/Qt5Quick")
set(Qt5QuickConfig_DIR "${QTDIR}/lib/cmake/Qt5QuickConfig")
set(Qt5Test_DIR "${QTDIR}/lib/cmake/Qt5Test")
set(Qt5Sensors_DIR "${QTDIR}/lib/cmake/Qt5Sensors")
set(Qt5Svg_DIR "${QTDIR}/lib/cmake/Qt5Svg")
set(Qt5Widgets_DIR "${QTDIR}/lib/cmake/Qt5Widgets")
