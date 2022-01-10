cmake_minimum_required(VERSION 3.0)

add_library(ldArrow)

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(ldArrow PUBLIC Qt5::Core)

target_include_directories(ldArrow PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ldArrow PRIVATE ldCore::LuaGame)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(ldArrow PRIVATE
   ${SRCDIR}/ldArrow.cpp
   ${SRCDIR}/ldArrow.h
   ${SRCDIR}/vis/ldArrowVisualizer.cpp
   ${SRCDIR}/vis/ldArrowVisualizer.h
   ${CMAKE_CURRENT_LIST_DIR}/res/arrow.qrc
    )
