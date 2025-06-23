# can be done only after project() call!
if(WIN32)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(LD_WINX64_BUILD ON)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(LD_WINX64_BUILD OFF)
endif()

message(STATUS "LD_WINX64_BUILD: ${LD_WINX64_BUILD}")

endif()
