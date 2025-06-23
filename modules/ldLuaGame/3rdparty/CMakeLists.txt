cmake_minimum_required(VERSION 3.10)

# lua
add_subdirectory(lua)

# sol2
option(SOL2_ENABLE_INSTALL "" OFF)
add_subdirectory(sol2)
mark_as_advanced(
    SOL2_CI
    SOL2_SYSTEM_INCLUDE
    SOL2_TESTS
    SOL2_EXAMPLES
    SOL2_INTEROP_EXAMPLES
    SOL2_DYNAMIC_LOADING_EXAMPLES
    SOL2_SINGLE
    SOL2_DOCS
    SOL2_ENABLE_INSTALL
)

