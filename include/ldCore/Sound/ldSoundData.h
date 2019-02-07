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

//
//  ldSoundData.h
//  ldCore
//
//  Created by rock on 5/20/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldSoundData__
#define __ldCore__ldSoundData__
#include "ldCore/ldCore_global.h"

#include <math.h>

#include <QtCore/QObject>
#include <QtMultimedia/QAudio>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/qaudioinput.h>

#include <ldCore/Utilities/ldBasicDataStructures.h>

// sample size is size of an audio buffer object in frames
const int SAMPLE_RATE = 44100;
const int SAMPLE_SIZE = SAMPLE_RATE / 30; // 1470

// overdrive factor increases the update speed of audio without affecting the size of the buffer
const int AUDIO_OVERDRIVE_FACTOR = 2;

// block size is the size of a full frame in samples
const int AUDIO_BLOCK_SIZE = SAMPLE_SIZE*2; // 2940

// sub block size corresponds to the number of new samples in each update
const int AUDIO_SUB_BLOCK_SIZE  = AUDIO_BLOCK_SIZE/AUDIO_OVERDRIVE_FACTOR; // 1470

// duration of time between audio block update
constexpr float AUDIO_UPDATE_DELTA_S = (AUDIO_SUB_BLOCK_SIZE/2) / 44100.0f; // 0.016
constexpr float AUDIO_UPDATE_DELTA_MS = AUDIO_UPDATE_DELTA_S * 1000; // 16.6

// structures for audio block and sub block
typedef struct AudioBlock {
    static const AudioBlock EMPTY_AUDIO_BLOCK;

    AudioBlock(bool clear = false);
    float data[AUDIO_BLOCK_SIZE];
} AudioBlock;

typedef struct AudioSubBlock {float data[AUDIO_SUB_BLOCK_SIZE];} AudioSubBlock;

// MH Note: 2940 @30fps = 88200 samples / sec... it would appear we are leaving half the buffer empty...
// we need to read 2940 interleved samples per frame, but we store them into deinterleved buffers half this length
// A SAMPLE_SIZE of 2940 means all buffers are twice as long as they need to be, question is is this by design or accident?

/**
 -----------------------------------
 The Frquency table of musical scale
 -----------------------------------
 C1 ~ G1# -  32.70 Hz ~  51.91 Hz
 A1 ~ G2# -  55.00 Hz ~ 103.83 Hz
 A2 ~ G3# - 110.00 Hz ~ 207.65 Hz
 A3 ~ G4# - 220.00 Hz ~ 415.30 Hz
 A4 ~ G5# - 440.00 Hz ~ 860.61 Hz
 -----------------------------------
 */
#define USING_MAX_FREQ (369.99) // F4# - We think this is the supremum of high freqency

// MH this value should be 10x higher given where it is used in powerAt().
// I've compensated for this discrepancy in the powerAt() implementation


class LDFFT;

class LDCORESHARED_EXPORT ldSoundData : public QObject
{
    Q_OBJECT

public:

    ldSoundData(const QAudioFormat &format, QObject* parent = 0);
    ~ldSoundData();


public:

    /// Update sound data for current playing data    
    void Update(const AudioBlock& block);

public:
    /**
     Gets the power value of left channel

     @return the value between -1 and 1
     */
    float GetWaveformL() const;

    /**
     Gets the power value of right channel.

     @return the value between -1 and 1
     */

    float GetWaveformR() const;

    /**
     Gets the power value of left channel

     @param the seek value, must be between 0 and SAMPLE_SIZE

     @return the value between -1 and 1
     */
    float GetWaveformL(int seek) const;

    /**
     Gets the power value of right channel.

     @param the seek value, must be between 0 and SAMPLE_SIZE

     @return the value between -1 and 1

     MH: Appears to return values between -0.2222215..0.2222215

     */

    float GetWaveformR(int seek) const;

    /**
     Gets the power value of low frequency range.

     @return the value between 0 and 1

     MH:Appears to return  values between -0.2222215..0.2222215
     */
    float GetBass() const;

    /**
     Gets the power value of middle frequency range.

     @return the value between 0 and 1
     */
    float GetMids() const;

    /**
     Gets the power value of high frequency range.

     @return the value between 0 and 1
     */
    float GetHigh() const;

    /**
     Gets the power value of frequency.

     @param frequency, this value must be greater or equals than 0 and be less or equals than the half of the music's sample rate

     @return the value between 0 and 1
     */
    float GetFFTValue(int frequency) const;


    /**
     gets the power of frequency block that is specified by i parameter.
     the function divides the entire frequency range into blocks of "width" count.
     then returns the "i"th block of them.
     i.e : i = 2, width = 20, then, we divide the freq range into 20 blocks, and return 2nd block.

     @param i       the index of frequency block
     @param width   the count of frequency blocks

     @return the power of frequency between 0 and 1
     */
    float powerAt(int i, int width) const;

    /**

     Provide direct access to waveform buffers.


     @return  pointer to array of values in range -1..1

     */


    int GetWaveformBufferLength() const { return SAMPLE_SIZE; }

    float * GetWaveformBufferL() { return m_waveform.GetLeft();  }
    float * GetWaveformBufferR() { return m_waveform.GetRight(); }

    float GetFFTInFreqRange(float startHz, float endHz, bool normalize, bool weight, bool max) const;
    void GetFullFFT(std::vector<float> &fftArray, bool normalize, bool weight) const;

    /**

     not supported
     Gets the left & right average audio levels.

     @param left,  float pointer, on return contains the level for the left channel
     @param right, float pointer, on return contains the level for the right channel

     @return  left & right will contain a value between 0..1

     */
    //void GetLevels(float *left, float *right);

    /**

     not supported
     Gets the left & right audio average and peak levels.

     @param buffer,  float buffer with >= 4 elements

     @return  buffers first 4 elements will contain a value between 0..1 [avgL, peakL, avgR, peakR]

     */
    //void GetLevels(float *buffer);

    /**
     Gets the single FFT value closest to frequencyInHertz.

     @param frequencyInHertz,  range 0..22050

     @return  value in range 0..1

     */
    float GetFFTValueForFrequency(float frequencyInHertz) const {
            return GetFFTValueForFrequency(frequencyInHertz, 0);
    }

    /**

     Gets the FFT value closest to frequencyInHertz, but sums <windowSize> adjoining bins on
     either side of the target bin and returns their average.

     @param frequencyInHertz,  range 0..22050
     @param windowSize,  range 0..SAMPLE_SIZE/2

     @return  value in range 0..1

     */
    float GetFFTValueForFrequency(float frequencyInHertz, unsigned windowSize) const;

    /**

     Gets the average FFT value for an indexed block.
     This method uses bins 0..370 to return values for frequencies from 0..5.5kHz
     The bins are divided into numBlocks, and then the values for all bins in block[blockIndex] are summed and averaged

     @param blockIndex,  range 0..numBlocks
     @param numBlocks,   range 1..SAMPLE_SIZE/2

     @return  value in range 0..1

     */
    float GetFFTValueForBlock(unsigned blockIndex, unsigned numBlocks) const;

    /**

     Fills a passed buffer with a spectrum of averaged FFT values between the min & max HZ.
     The total number of bins covering the frequency range are divided into spectrumLength number of blocks,
     and then the values for each bin in a block are summed and averaged

     @param spectrum,  float buffer with size >= spectrumLength
     @param spectrumLength,   range 1..SAMPLE_SIZE/2
     @param minHz,  range 0..22500
     @param maxHz,  range minHz..22500

     @return  void, spectrum is filled to spectrumLength with values in range 0..1
     */
    void GetSpectrum(float *spectrum, unsigned spectrumLength, float minHz, float maxHz) const;

    /**

     Fills a passed buffer with a spectrum of averaged FFT values for frequencies between 100 & 4000 HZ.

     @param spectrum,  float buffer with size >= spectrumLength
     @param spectrumLength,   range 1..SAMPLE_SIZE/2

     @return  void, spectrum is filled to spectrumLength with values in range 0..1

     */
    void GetMusicSpectrum(float *spectrum, unsigned spectrumLength) const {
        GetSpectrum(spectrum, spectrumLength, 100.0, 4000.0);
    }


    float GetSampleRate() const { return soundMaxFrequency * 2.0f; }

    int GetSoundLevel() const; // 1..100

protected:

    const QAudioFormat m_format;

    quint32 m_maxAmplitude;

    QScopedPointer<LDFFT>                   m_fft;
    ldAudioBuffer<float>    m_waveform; // waveform data buffer
    ldBuffer<float>         m_frequency; // fruenqcy data buffer
    ldBuffer<short>         m_rawdata; // raw data buffer

    float   lastUpdateTime;
    int     lastUpdateSeek;

    int     soundMaxFrequency;
    float   frequencyGap;

    float _testTheta, _testFrequency; // Used by the Sine-Wave synthesizer

    float _averageL, _averageR, _peakL, _peakR, _minL, _minR; // average & peak levels



    // volume correction    
    void volumeCorrectionInit();    
    void volumeCorrectionApply();
public:    
    float volumePowerPre;
    float volumePowerPost;    
    float volumePowerLast;
    float volumePowerLastDB;
    float volumePowerLastDBScaled;
    float volumePowerMaxDBScaled;
    float volumePowerMaxTimer;
    float volumePowerSmoothedDBScaled;    
    float volumePowerAutoGainDB;
    float volumePowerAutoGainDBScaled;
    float volumePowerAutoGainFactor;
    float volumePowerClipCount;
    
    
    //DCRejectionFilter filterL, filterR; // Removes DC component prior to FFT for cleaner results
};

#endif /* defined(__ldCore__ldSoundData__) */
