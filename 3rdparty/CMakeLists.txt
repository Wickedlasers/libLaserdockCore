cmake_minimum_required(VERSION 3.11)

project(3rdparty)

# should be the first one because some libraries depends on OpenMP
if(LD_CORE_ENABLE_OPENMP)
    add_subdirectory(openmp)
endif()

add_subdirectory(aubio)
add_subdirectory(kiss_fft130)
add_subdirectory(lib-qt-qml-tricks)
add_subdirectory(laserdocklib)
    mark_as_advanced(
        LASERDOCKLIB_INSTALL
        )
add_subdirectory(libaudiodecoder)
add_subdirectory(nanosvg-master)
add_subdirectory(openlase)
add_subdirectory(simplecrypt)

if(LD_CORE_ENABLE_LIBLZR)
    add_subdirectory(liblzr)
endif()

if(LD_CORE_ENABLE_QUAZIP)
    add_subdirectory(quazip)
endif()
