cmake_minimum_required(VERSION 3.11)

add_library(libqtqmltricks)

SET(CMAKE_C_ARCHIVE_CREATE   "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
if(CMAKE_GENERATOR STREQUAL Xcode)
    SET(CMAKE_C_ARCHIVE_FINISH   "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
    SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
endif()

target_include_directories(libqtqmltricks PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(libqtqmltricks PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src)

set(Qt${QT_VERSION_MAJOR}_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}")
set(Qt${QT_VERSION_MAJOR}Core_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Core")
set(Qt${QT_VERSION_MAJOR}Gui_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Gui")
set(Qt${QT_VERSION_MAJOR}Qml_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Qml")
set(Qt${QT_VERSION_MAJOR}Quick_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Quick")
set(Qt${QT_VERSION_MAJOR}Svg_DIR "${QTDIR}/lib/cmake/Qt${QT_VERSION_MAJOR}Svg")
set(CMAKE_PREFIX_PATH "${QTDIR}/lib/cmake")
find_package(Qt${QT_VERSION_MAJOR} CONFIG REQUIRED Core Gui Qml Quick Svg)
target_link_libraries(libqtqmltricks PRIVATE Qt::Core Qt::Gui Qt::Qml Qt::Quick Qt${QT_VERSION_MAJOR}::Svg) # Qt::Svg doesn't work in Qt6.3, Qt bug

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

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::libqtqmltricks ALIAS libqtqmltricks)
endif()
