cmake_minimum_required ( VERSION 3.11 )

project ( lua C )

include_directories ( src ${CMAKE_CURRENT_BINARY_DIR} )
set ( SRC_CORE src/lapi.c src/lcode.c src/lctype.c src/ldebug.c src/ldo.c src/ldump.c src/lfunc.c src/lgc.c src/llex.c
  src/lmem.c src/lobject.c src/lopcodes.c src/lparser.c src/lstate.c src/lstring.c src/ltable.c
  src/ltm.c src/lundump.c src/lvm.c src/lzio.c )
set ( SRC_LIB src/lauxlib.c src/lbaselib.c src/lbitlib.c src/lcorolib.c src/ldblib.c src/liolib.c
  src/lmathlib.c src/loslib.c src/lstrlib.c src/ltablib.c src/lutf8lib.c src/loadlib.c src/linit.c )

set ( SRC_LUA src/lua.c )
set ( SRC_LUAC src/luac.c )

add_library ( liblua ${SRC_CORE} ${SRC_LIB} )
set_target_properties ( liblua PROPERTIES OUTPUT_NAME lua )

set_property(TARGET liblua PROPERTY AUTOMOC OFF)
set_property(TARGET liblua PROPERTY AUTORCC OFF)
set_property(TARGET liblua PROPERTY AUTOUIC OFF)

target_include_directories(liblua PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)

if(WIN32)
#    add_library ( libluadll SHARED ${SRC_CORE} ${SRC_LIB} )
#    target_compile_definitions ( libluadll PRIVATE _CRT_SECURE_NO_WARNINGS LUA_BUILD_AS_DLL )
#    set_target_properties ( libluadll PROPERTIES OUTPUT_NAME lua53 )

    # disable various conversion warnings
    target_compile_options(liblua PRIVATE /wd4244) # conversion from double to int
endif()

if(LINUX)
    target_link_libraries(liblua PRIVATE dl)
    target_compile_definitions(liblua PRIVATE LUA_USE_LINUX)
endif()

if(ANDROID)
    target_compile_definitions(liblua PRIVATE LUA_USE_LINUX)
endif()

if(APPLE)
    target_compile_definitions(liblua PRIVATE LUA_USE_MACOSX)

    target_compile_options(liblua PRIVATE -Wno-unused-private-field -Wno-implicit-function-declaration -Wno-unused-parameter -Wno-string-plus-int)
endif()
