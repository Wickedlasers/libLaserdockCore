macro(setup_cpp_flags TARGET)
if(UNIX)
    target_compile_options(${TARGET} PUBLIC -Wall -Wextra -Wuninitialized -Winit-self -Wno-unknown-pragmas -Wpedantic)

    # good to have some day...
#    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#        target_compile_options(${TARGET} PUBLIC -Weverything)
#        target_compile_options(${TARGET} PUBLIC -Wno-c++98-compat)
#        target_compile_options(${TARGET} PUBLIC -Wno-c++98-compat-pedantic)
#        target_compile_options(${TARGET} PUBLIC -Wno-unused-macros)
#        target_compile_options(${TARGET} PUBLIC -Wno-newline-eof)
#        target_compile_options(${TARGET} PUBLIC -Wno-exit-time-destructors)
#        target_compile_options(${TARGET} PUBLIC -Wno-global-constructors)
#        target_compile_options(${TARGET} PUBLIC -Wno-gnu-zero-variadic-macro-arguments)
#        target_compile_options(${TARGET} PUBLIC -Wno-documentation)
#        target_compile_options(${TARGET} PUBLIC -Wno-shadow)
#        target_compile_options(${TARGET} PUBLIC -Wno-switch-enum)
#        target_compile_options(${TARGET} PUBLIC -Wno-missing-prototypes)
#        target_compile_options(${TARGET} PUBLIC -Wno-used-but-marked-unused)
#        target_compile_options(${TARGET} PUBLIC -Wno-inconsistent-missing-destructor-override)
#        target_compile_options(${TARGET} PUBLIC -Wno-padded)
#        target_compile_options(${TARGET} PUBLIC -Wno-zero-as-null-pointer-constant)
#    endif()

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
