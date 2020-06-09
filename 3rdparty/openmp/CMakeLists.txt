# macOS
if(APPLE)
    add_library(openmp SHARED IMPORTED GLOBAL)
    set_property(TARGET openmp PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include/mac")
    set_property(TARGET openmp PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_LIST_DIR}/lib/mac/libomp.dylib")
    target_compile_options(openmp INTERFACE -Xpreprocessor)
    target_compile_options(openmp INTERFACE -fopenmp)
endif()

# Android
if(ANDROID)
    add_library(openmp INTERFACE IMPORTED GLOBAL)
    target_link_libraries(openmp INTERFACE -static-openmp -fopenmp) # for NDK 21 static-openmp should be used because of bug
endif()

# Linux
if(LINUX)
    add_library(openmp INTERFACE IMPORTED GLOBAL)
    target_link_libraries(openmp INTERFACE -lgomp)
    target_compile_options(openmp INTERFACE -fopenmp)
endif()

if(WIN32)
    add_library(openmp INTERFACE IMPORTED GLOBAL)
    find_package(OpenMP REQUIRED)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(openmp INTERFACE OpenMP::OpenMP_CXX)
    endif()
    target_compile_options(openmp INTERFACE /openmp)
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::openmp ALIAS openmp)
endif()
