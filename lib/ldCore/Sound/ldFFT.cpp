/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#include "ldFFT.h"

LDFFT::LDFFT(unsigned size)
    : m_size(size)
    , m_fftin(size)
    , m_fftout(size)
{
    m_cfg = kiss_fft_alloc(size, 0, NULL, NULL);
//    printf("sizeof(kiss_fft_scalar) = %d\n",(int)sizeof(kiss_fft_scalar) );
}

LDFFT::~LDFFT()
{
    kiss_fft_free(m_cfg);
}

bool LDFFT::GetComplex(const ldAudioBuffer<float>& buffer, ldBuffer<float>& real, ldBuffer<float>& image)
{
    if (m_size != buffer.GetSize()) return false;
    if (m_size != real.GetSize()) return false;
    if (m_size != image.GetSize()) return false;

    // load data to FFT
    for (unsigned i = 0; i < m_size; ++i)
    {
//        m_fftin[i].r = buffer.GetLeft(i); // MH Note this should be the average of both channels
//        m_fftin[i].i = buffer.GetRight(i); // MH NOTE: This should be zero for real data

        m_fftin[i].r = (buffer.GetLeft(i) + buffer.GetRight(i)) * 0.5f;
        m_fftin[i].i = 0.0f;

    }
    // calculate FFT
    kiss_fft(m_cfg, m_fftin.GetData(), m_fftout.GetData());
    // save real and image parts
    for (unsigned i = 0; i < m_size; ++i)
    {
        real[i] = m_fftout[i].r;
        image[i] = m_fftout[i].i;
    }

    // MH Note: KISS FFT in float-mode scales output samples by SAMPLE_SIZE, so the canonical way of normalising FFT values
    // is to scale by 1/SAMPLE_SIZE. For Amplitudes it is also convention to further multiply by 2.0
    // As most existing visualizers have been set-up to expect values in the raw scaled range I have left the
    // FFT output with the original unscaled output.
    // I have instead added new methods to access the FFT spectral data via ldSoundData, these new spectral methods
    // all return canonically normalised un-clipped values.


    return true;
}

bool LDFFT::GetScalar(const ldAudioBuffer<float>& buffer, ldBuffer<float>& scalar)
{
    if (m_size != buffer.GetSize()) return false;
    if (m_size != scalar.GetSize()) return false;

    // load data to FFT

    for (unsigned i = 0; i < m_size; ++i)
    {
//        m_fftin[i].r = buffer.GetLeft(i); // MH Note this should be the average of both channels
//        m_fftin[i].i = buffer.GetRight(i); // MH NOTE: This should be zero for real data

        m_fftin[i].r = (buffer.GetLeft(i) + buffer.GetRight(i)) * 0.5f;
        m_fftin[i].i = 0.0f;


    }
    // calculate FFT
    kiss_fft(m_cfg, m_fftin.GetData(), m_fftout.GetData());
    // calculate scalar
    for (unsigned i = 0; i < m_size; ++i)
    {
        float _r = m_fftout[i].r;
        float _i = m_fftout[i].i;
        scalar[i] = sqrtf(_r * _r + _i * _i);
    }

    // MH Note: KISS FFT in float-mode scales output samples by SAMPLE_SIZE, so the canonical way of normalising FFT values
    // is to scale by 1/SAMPLE_SIZE. For Amplitudes it is also convention to further multiply by 2.0
    // As most existing visualizers have been set-up to expect values in the raw scaled range I have left the
    // FFT output with the original unscaled output.
    // I have instead added new methods to access the FFT spectral data via ldSoundData, these new spectral methods
    // all return canonically normalised un-clipped values.

    return true;
}

#ifdef Q_OS_MACOS
#pragma mark - LDFFTr: Real FFT
#endif

LDFFTr::LDFFTr(unsigned size)
: m_size(size)
, m_fftin(size)
, m_fftout(size / 2 + 1)
{
    m_cfg = kiss_fftr_alloc(size, 0, NULL, NULL);
    printf("sizeof(kiss_fft_scalar) = %d\n",(int)sizeof(kiss_fft_scalar) );
}

LDFFTr::~LDFFTr()
{
    kiss_fftr_free(m_cfg);
}

bool LDFFTr::GetComplex(const ldAudioBuffer<float>& buffer, ldBuffer<float>& real, ldBuffer<float>& image)
{
    if (m_size > buffer.GetSize()) return false;

    size_t outSize = m_size / 2 + 1;

    if (outSize > real.GetSize()) return false;
    if (outSize > image.GetSize()) return false;

    for (unsigned i = 0; i < m_size; ++i)
        m_fftin[i] = (buffer.GetLeft(i) + buffer.GetRight(i)) * 0.5;

    kiss_fftr(m_cfg, m_fftin.GetData(), m_fftout.GetData());

    float scale = 1.0 / (float)outSize;

    for (unsigned i = 0; i < m_size; ++i)
    {
        real[i] = m_fftout[i].r * scale;
        image[i] = m_fftout[i].i * scale;
    }

    return true;
}

bool LDFFTr::GetScalar(const ldAudioBuffer<float>& buffer, ldBuffer<float>& scalar)
{
    if (m_size > buffer.GetSize()) return false;

    size_t outSize = m_size / 2 + 1;

    float scale = 1.0 / (float)outSize;

    if (outSize > scalar.GetSize()) return false;

    for (unsigned i = 0; i < m_size; ++i)
        m_fftin[i] = (buffer.GetLeft(i) + buffer.GetRight(i)) * 0.5f;

    kiss_fftr(m_cfg, m_fftin.GetData(),  m_fftout.GetData());

    for (unsigned i = 0; i < outSize; ++i)
    {
        float _r = m_fftout[i].r * scale;
        float _i = m_fftout[i].i * scale;
        scalar[i] = sqrtf(_r * _r + _i * _i);
    }

    return true;
}

