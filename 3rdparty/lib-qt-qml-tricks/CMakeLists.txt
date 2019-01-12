cmake_minimum_required(VERSION 3.11)

add_library(libqtqmltricks)

SET(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")

target_include_directories(libqtqmltricks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(libqtqmltricks PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

set(Qt5_DIR "${QTDIR}/lib/cmake/Qt5")
set(Qt5Core_DIR "${QTDIR}/lib/cmake/Qt5Core")
set(Qt5Gui_DIR "${QTDIR}/lib/cmake/Qt5Gui")
set(Qt5Qml_DIR "${QTDIR}/lib/cmake/Qt5Qml")
set(Qt5Quick_DIR "${QTDIR}/lib/cmake/Qt5Quick")
set(Qt5Svg_DIR "${QTDIR}/lib/cmake/Qt5Svg")
find_package(Qt5 CONFIG REQUIRED Core Gui Qml Quick Svg)
target_link_libraries(libqtqmltricks PRIVATE Qt5::Core Qt5::Gui Qt5::Qml Qt5::Quick Qt5::Svg)

target_sources(libqtqmltricks PRIVATE
    src/qqmlgadgetlistmodel.cpp
    src/qqmlgadgetlistmodel.h
    src/qqmlhelpers.cpp
    src/qqmlhelpers.h
    src/qqmlmodels.h
    src/qqmlobjectlistmodel.cpp
    src/qqmlobjectlistmodel.h
    src/qqmlsvgiconhelper.cpp
    src/qqmlsvgiconhelper.h
    src/qqmlvariantlistmodel.cpp
    src/qqmlvariantlistmodel.h
    src/qqmlvariantlistmodel_p.h
    src/qquickpolygon.cpp
    src/qquickpolygon.h
    src/qtbitstream.h
    src/qtcobs.h
    src/qtjsonpath.h
    src/qqmlvariantlisthelper.h
    )

