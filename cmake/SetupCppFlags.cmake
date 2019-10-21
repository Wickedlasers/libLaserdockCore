macro(setup_cpp_flags TARGET)
if(UNIX)
    target_compile_options(${TARGET} PUBLIC -Wall -Wextra -Wuninitialized -Winit-self -Wno-unknown-pragmas -Wpedantic)
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        target_compile_options(${TARGET} PUBLIC -fPIC)
    endif()
    if(NOT ANDROID)
        target_compile_options(${TARGET} PUBLIC -Wshadow)
    endif()
endif()

if(WIN32)
    # disable various conversion warnings
    target_compile_options(${TARGET} PUBLIC /wd4018) # signed/unsigned mismatch
    target_compile_options(${TARGET} PUBLIC /wd4244) # conversion from double to int
    target_compile_options(${TARGET} PUBLIC /wd4267) # conversion from size_t to uint16_t
    target_compile_options(${TARGET} PUBLIC /wd4305) # conversion from double to float

    target_compile_options(${TARGET} PUBLIC /wd4250) # diamond inheritance issue

    target_compile_options(${TARGET} PUBLIC /wd4566) # character represented by universal-character-name '\u3164' cannot be represented in the current code page (1251)
endif()

endmacro()
