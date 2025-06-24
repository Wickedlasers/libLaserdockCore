cmake_minimum_required(VERSION 3.10)

add_library(ldPong)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

target_include_directories(ldPong PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ldPong PUBLIC Qt::Core ldCore)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(ldPong PRIVATE
   ${SRCDIR}/ldPong.cpp
   ${SRCDIR}/ldPong.h
   ${SRCDIR}/vis/ldPongVisualizer.cpp
   ${SRCDIR}/vis/ldPongVisualizer.h
    )
