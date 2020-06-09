# desktop akforrest/quazip.git
# android stachenov/quazip

add_library(quazip SHARED IMPORTED GLOBAL)

if(ANDROID)
    include(android/CMakeLists.txt)
else()
    include(desktop/CMakeLists.txt)
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::quazip ALIAS quazip)
endif()
