set(CMAKE_AUTOMOC OFF)

# sources
set(LIBFFT_SOURCE_FILES
    _kiss_fft_guts.h
    kiss_fft.c
    kiss_fft.h
    kissfft.hh
    tools/kiss_fftr.c
    tools/kiss_fftr.h
        )

# library
add_library(kiss_fft130 ${LIBFFT_SOURCE_FILES})
