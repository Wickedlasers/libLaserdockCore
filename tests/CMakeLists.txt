cmake_minimum_required(VERSION 3.11)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

project(ldCore_Tests)

enable_testing()

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ldCore/Helpers/Maths/ldMathsTests.cpp
    ldCore/Helpers/Maths/ldMathsTests.h
    )

find_package(Qt5 CONFIG REQUIRED Core Test)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Core Qt5::Test ldCore)

add_test(${PROJECT_NAME} ${PROJECT_NAME})
