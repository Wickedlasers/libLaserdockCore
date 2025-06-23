cmake_minimum_required(VERSION 3.11)

add_library(abletonlink INTERFACE)

include(link/AbletonLinkConfig.cmake)

# header-only library
target_include_directories(abletonlink INTERFACE link/include)
target_include_directories(abletonlink INTERFACE link/modules/asio-standalone/asio/include)

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::abletonlink ALIAS abletonlink )
endif()
