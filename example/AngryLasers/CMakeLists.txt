cmake_minimum_required(VERSION 3.0)

add_library(ldAngryLasers)

find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
target_link_libraries(ldAngryLasers PUBLIC Qt::Core)

target_include_directories(ldAngryLasers PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(ldAngryLasers PRIVATE ldCore ldCore::Box2D)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(ldAngryLasers PRIVATE
   ${SRCDIR}/ldAngryLasers.cpp
   ${SRCDIR}/ldAngryLasers.h
   ${SRCDIR}/vis/ldAngryLasersVisualizer.cpp
   ${SRCDIR}/vis/ldAngryLasersVisualizer.h
   ${SRCDIR}/vis/impl/ldGradientObject.cpp
   ${SRCDIR}/vis/impl/ldGradientObject.h
   ${SRCDIR}/vis/impl/ldAngryLasersPlayer.cpp
   ${SRCDIR}/vis/impl/ldAngryLasersPlayer.h
   ${SRCDIR}/vis/impl/ldAngryLasersBox.cpp
   ${SRCDIR}/vis/impl/ldAngryLasersBox.h
   ${SRCDIR}/vis/impl/ldAngryLasersEnemy.cpp
   ${SRCDIR}/vis/impl/ldAngryLasersEnemy.h
   ${SRCDIR}/vis/impl/spline.cpp
   ${SRCDIR}/vis/impl/spline.h
   ${SRCDIR}/vis/impl/globals.h
   ${SRCDIR}/vis/impl/ldAngryLasersBox.h
   ${SRCDIR}/vis/impl/ldAngryLasersPlayer.h
   ${SRCDIR}/vis/impl/spline.h

)
