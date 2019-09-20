cmake_minimum_required(VERSION 3.11)

add_library(aubio)

set_property(TARGET aubio PROPERTY AUTOMOC OFF)
set_property(TARGET aubio PROPERTY AUTORCC OFF)
set_property(TARGET aubio PROPERTY AUTOUIC OFF)

target_sources(aubio PRIVATE
    src/aubio.h
    src/aubio_priv.h
    src/config.h
    src/cvec.c
    src/cvec.h
    src/fmat.c
    src/fmat.h
    src/fvec.c
    src/fvec.h
    src/lvec.c
    src/lvec.h
    src/mathutils.c
    src/mathutils.h
    src/musicutils.h
    src/types.h
    src/vecutils.c
    src/vecutils.h
#    src/io/audio_unit.c
    src/io/audio_unit.h
    src/io/ioutils.c
    src/io/ioutils.h
    src/io/sink.c
    src/io/sink.h
#    src/io/sink_apple_audio.c
    src/io/sink_apple_audio.h
#    src/io/sink_sndfile.c
    src/io/sink_sndfile.h
    src/io/sink_wavwrite.c
    src/io/sink_wavwrite.h
    src/io/source.c
    src/io/source.h
#    src/io/source_apple_audio.c
    src/io/source_apple_audio.h
#    src/io/source_avcodec.c
    src/io/source_avcodec.h
#    src/io/source_sndfile.c
    src/io/source_sndfile.h
    src/io/source_wavread.c
    src/io/source_wavread.h
#    src/io/utils_apple_audio.c
    src/notes/notes.c
    src/notes/notes.h
    src/onset/onset.c
    src/onset/onset.h
    src/onset/peakpicker.c
    src/onset/peakpicker.h
    src/pitch/pitch.c
    src/pitch/pitch.h
    src/pitch/pitchfcomb.c
    src/pitch/pitchfcomb.h
    src/pitch/pitchmcomb.c
    src/pitch/pitchmcomb.h
    src/pitch/pitchschmitt.c
    src/pitch/pitchschmitt.h
    src/pitch/pitchspecacf.c
    src/pitch/pitchspecacf.h
    src/pitch/pitchyin.c
    src/pitch/pitchyin.h
    src/pitch/pitchyinfast.c
    src/pitch/pitchyinfast.h
    src/pitch/pitchyinfft.c
    src/pitch/pitchyinfft.h
    src/spectral/awhitening.c
    src/spectral/awhitening.h
    src/spectral/fft.c
    src/spectral/fft.h
    src/spectral/filterbank.c
    src/spectral/filterbank.h
    src/spectral/filterbank_mel.c
    src/spectral/filterbank_mel.h
    src/spectral/mfcc.c
    src/spectral/mfcc.h
    src/spectral/ooura_fft8g.c
    src/spectral/phasevoc.c
    src/spectral/phasevoc.h
    src/spectral/specdesc.c
    src/spectral/specdesc.h
    src/spectral/statistics.c
    src/spectral/tss.c
    src/spectral/tss.h
    src/synth/sampler.c
    src/synth/sampler.h
    src/synth/wavetable.c
    src/synth/wavetable.h
    src/tempo/beattracking.c
    src/tempo/beattracking.h
    src/tempo/tempo.c
    src/tempo/tempo.h
    src/temporal/a_weighting.c
    src/temporal/a_weighting.h
    src/temporal/biquad.c
    src/temporal/biquad.h
    src/temporal/c_weighting.c
    src/temporal/c_weighting.h
    src/temporal/filter.c
    src/temporal/filter.h
    src/temporal/resampler.c
    src/temporal/resampler.h
    src/utils/hist.c
    src/utils/hist.h
    src/utils/log.c
    src/utils/log.h
    src/utils/parameter.c
    src/utils/parameter.h
    src/utils/scale.c
    src/utils/scale.h
    )


target_include_directories(aubio PUBLIC src)

if(WIN32)
    target_compile_definitions(aubio PRIVATE _USE_MATH_DEFINES)
endif()


if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_compile_options(aubio PRIVATE -fPIC)
endif()

if(NOT CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    add_library(${PROJECT_NAME}::aubio ALIAS aubio)
endif()
