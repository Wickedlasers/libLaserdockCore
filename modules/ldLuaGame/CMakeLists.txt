cmake_minimum_required(VERSION 3.0)

option(LD_ENABLE_LUA_LOG "Enable lua log" ON)

add_subdirectory(3rdparty)

add_library(ldLuaGame)

target_include_directories(ldLuaGame PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)
set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/include)

target_sources(ldLuaGame PRIVATE
    ${SRCDIR}/ldLuaGameVisualizer.cpp
    ${INCDIR}/ldLuaGame/ldLuaGameVisualizer.h
    ${CMAKE_CURRENT_LIST_DIR}/res/lua_base.qrc
    )

target_link_libraries(ldLuaGame PUBLIC ldCore liblua sol2)

if(LD_ENABLE_LUA_LOG)
    target_compile_definitions(ldLuaGame PRIVATE LD_ENABLE_LUA_LOG)
endif()

add_library(${PROJECT_NAME}::LuaGame ALIAS ldLuaGame)
