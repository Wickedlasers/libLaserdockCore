cmake_minimum_required(VERSION 3.0)

add_library(ldSerpent)

target_include_directories(ldSerpent PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ldSerpent PRIVATE ldCore ldRazer)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(ldSerpent PRIVATE
   ${SRCDIR}/ldSerpent.cpp
   ${SRCDIR}/ldSerpent.h
   ${SRCDIR}/vis/ldSerpentVisualizer.cpp
   ${SRCDIR}/vis/ldSerpentVisualizer.h
   ${SRCDIR}/vis/impl/ldSerpentCommon.h
   ${SRCDIR}/vis/impl/ldSerpentGame.h
   ${SRCDIR}/vis/impl/ldSerpentGame.cpp
   ${SRCDIR}/vis/impl/ldSerpentMaze.h
   ${SRCDIR}/vis/impl/ldSerpentMaze.cpp
   ${SRCDIR}/vis/impl/ldSerpentMazeGenerator.h
   ${SRCDIR}/vis/impl/ldSerpentMazeGenerator.cpp
   ${SRCDIR}/vis/impl/ldSerpentPickup.h
   ${SRCDIR}/vis/impl/ldSerpentPickup.cpp
   ${SRCDIR}/vis/impl/ldSnakeObj.h
   ${SRCDIR}/vis/impl/ldSnakeObj.cpp
    )
