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

#ifndef LDFFT_INCLUDED
#define LDFFT_INCLUDED

#include <kiss_fft130/kiss_fft.h>
#include <kiss_fft130/tools/kiss_fftr.h>

#include <ldCore/Utilities/ldBasicDataStructures.h>

/** Actual workhourse of the sound signal processing. Does discrete fourier transformation on sound data. */
class LDFFT
{
public:
    LDFFT(unsigned size);
    ~LDFFT();

    /*--------------------------------------------------//
     Compute Complex FFT and get results as complex
     [parameters]
     buffer - audio data buffer
     real   - output real part buffer
     image  - output image part buffer
     [return]
     true if FFT was computed or false in other case
    //--------------------------------------------------*/
    bool GetComplex(const ldAudioBuffer<float>& buffer, ldBuffer<float>& real, ldBuffer<float>& image);
    /*--------------------------------------------------//
     Compute Complex FFT and get results as scalar (module of complex)
     [parameters]
     buffer - audio data buffer
     scalar - output scalar buffer
     [return]
     true if FFT was computed or false in other case
    //--------------------------------------------------*/
    bool GetScalar(const ldAudioBuffer<float>& buffer, ldBuffer<float>& scalar);

    bool GetScalarReal(const ldAudioBuffer<float>& buffer, ldBuffer<float>& scalar);

private:
    unsigned                m_size;
    kiss_fft_cfg            m_cfg;
    ldBuffer<kiss_fft_cpx>  m_fftin;
    ldBuffer<kiss_fft_cpx>  m_fftout;

};


/*--------------------------------------------------//
 LDFFTr computes the Real FFT.
 This should compute approx 2x faster than the Complex FFT
 //--------------------------------------------------*/

class LDFFTr
{
public:
    LDFFTr(unsigned size);
    ~LDFFTr();

    /*--------------------------------------------------//
     Compute Real FFT and get results as complex
     [parameters]
     buffer - audio data buffer
     real   - output real part buffer
     image  - output image part buffer
     [return]
     true if FFT was computed or false in other case
     //--------------------------------------------------*/
    bool GetComplex(const ldAudioBuffer<float>& buffer, ldBuffer<float>& real, ldBuffer<float>& image);
    /*--------------------------------------------------//
     Compute Real FFT and get results as scalar (module of complex)
     [parameters]
     buffer - audio data buffer
     scalar - output scalar buffer
     [return]
     true if FFT was computed or false in other case
     //--------------------------------------------------*/
    bool GetScalar(const ldAudioBuffer<float>& buffer, ldBuffer<float>& scalar);


private:

    unsigned                m_size;

    kiss_fftr_cfg           m_cfg;

    ldBuffer<float>  m_fftin;
    ldBuffer<kiss_fft_cpx>  m_fftout;


};

#endif // LDFFT_INCLUDED

