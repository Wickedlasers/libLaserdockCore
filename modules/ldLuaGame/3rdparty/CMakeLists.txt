cmake_minimum_required(VERSION 3.0)

add_subdirectory(lua)
add_subdirectory(sol2)
mark_as_advanced(
    SOL2_LUA_VERSION
    SOL2_BUILD_LUA
    SOL2_PLATFORM
    SOL2_CI
    SOL2_TESTS
    SOL2_EXAMPLES
    SOL2_INTEROP_EXAMPLES
    SOL2_DYNAMIC_LOADING_EXAMPLES
    SOL2_GENERATE_SINGLE
    SOL2_SINGLE
    SOL2_DOCS
    )

