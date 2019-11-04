cmake_minimum_required(VERSION 3.0)

add_library(ldSpiralFighter)

target_include_directories(ldSpiralFighter PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ldSpiralFighter PRIVATE ldCore)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(ldSpiralFighter PRIVATE
   ${SRCDIR}/ldSpiralFighterGame.cpp
   ${SRCDIR}/ldSpiralFighterGame.h
   ${SRCDIR}/vis/ldSpiralFighterVisualizer.cpp
   ${SRCDIR}/vis/ldSpiralFighterVisualizer.h
   ${SRCDIR}/vis/impl/ldSpiralFighterEnemy.cpp
   ${SRCDIR}/vis/impl/ldSpiralFighterEnemy.h
   ${SRCDIR}/vis/impl/ldSpiralFighterMissile.cpp
   ${SRCDIR}/vis/impl/ldSpiralFighterMissile.h
   ${SRCDIR}/vis/impl/ldSpiralFighterPlayer.cpp
   ${SRCDIR}/vis/impl/ldSpiralFighterPlayer.h
    )
