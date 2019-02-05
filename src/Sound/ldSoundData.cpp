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
//  ldSoundData.cpp
//  ldCore
//
//  Created by rock on 5/20/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#include <math.h>
#include <QtCore/QDebug>
#include <QtCore/qendian.h>

#include "ldCore/Sound/ldSoundData.h"
#include "ldCore/Sound/ldSoundInterface.h"
#include <ldCore/Utilities/ldBasicDataStructures.h>
#include <ldCore/Utilities/ldUtils.h>

#include "ldFFT.h"


/*!
  \class ldSoundData
  \brief The sound data classes encapsulates commonly used data regarding a given audio data segment, primarily regarding its frequency content.
  \inmodule audio

  This class is used to analyze a segment of audio data and extract relevant data from it.
  This data can then be used to generate live visualization.
*/

const AudioBlock AudioBlock::EMPTY_AUDIO_BLOCK = AudioBlock(true);



AudioBlock::AudioBlock(bool clear)
{
    if(clear) {
        for(int i = 0; i < AUDIO_BLOCK_SIZE; ++i)
            data[i] = 0.;
    }
}

/*!
 * \brief ldSoundData::ldSoundData
 */
ldSoundData::ldSoundData(const QAudioFormat &format, QObject* parent) :
    QObject(parent),
    m_format(format),
    m_maxAmplitude(0),
    m_fft(new LDFFT(SAMPLE_SIZE)),
    m_waveform(SAMPLE_SIZE),
    m_frequency(SAMPLE_SIZE),
    m_rawdata(SAMPLE_SIZE * 2),
    soundMaxFrequency(44100/2), // nyquist
    frequencyGap(1.0f/30.0f)
{
    volumeCorrectionInit();

    switch (m_format.sampleSize()) {
    case 8:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 255;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 127;
            break;
        default:
            break;
        }
        break;
    case 16:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 65535;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 32767;
            break;
        default:
            break;
        }
        break;

    case 32:
        switch (m_format.sampleType()) {
        case QAudioFormat::UnSignedInt:
            m_maxAmplitude = 0xffffffff;
            break;
        case QAudioFormat::SignedInt:
            m_maxAmplitude = 0x7fffffff;
            break;
        case QAudioFormat::Float:
            m_maxAmplitude = 0x7fffffff; // Kind of
        default:
            break;
        }
        break;

    default:
        m_maxAmplitude = (float) 0x8FFF; // default value
        break;
    }
}

/*!
 * \brief ldSoundData::~ldSoundData
 */
ldSoundData::~ldSoundData()
{

}

/*
 * ldSoundData::Update
 * new version of update function
 * takes in an AudioBlock type
 * note: incomplete implementation
 * does not compute averageL,_peakL (are these used anywhere?)
*/
void ldSoundData::Update(const AudioBlock &block) {


    float tMaxL = 0.0f, tSumL = 0.0f, tMaxR = 0.0f, tSumR = 0.0f, tAbs, scale = 1.0f / SAMPLE_SIZE;

    for (int i = 0; i < SAMPLE_SIZE; i++) {

        // get left and right from block
        float tLeft = block.data[i*2+0];
        float tRight = block.data[i*2+1];
        m_waveform.GetLeft()[i]  = tLeft;
        m_waveform.GetRight()[i] = tRight;

        // Accumulate peak & average power
        tAbs = fabs(tLeft); if(tAbs > tMaxL) tMaxL = tAbs; tSumL += tAbs;
        tAbs = fabs(tRight); if(tAbs > tMaxR) tMaxR = tAbs; tSumR += tAbs;

    }

    // Average & peak are stored squared leaving sqrtf to JIT
    _averageL = tSumL * scale;
    _averageR = tSumR * scale;
    _peakL = tMaxL;
    _peakR = tMaxR;

    // Perform FFT, storing results in mFrequency
    m_fft->GetScalar(m_waveform, m_frequency);

    volumeCorrectionApply();
}

// auto scale params
static const float mindb = -20;                             // quietest signal
static const float maxdb = 0* 2 * 3.16227766016838;         // loudest signal
static const float targetdb = -2 * 3.16227766016838f;        // desired strength (headroom)
static const float ceildb = -0.0/10.0 * 3.16227766016838;   // hard cap for output signal
static const float falltime = 25;                           // duration in seconds for volume fall down to to silent
static const float risetime = 10;                            // duration for volume to rise after a loud sound (approx)

void ldSoundData::volumeCorrectionInit() {     
    volumePowerPre = 0.5;
    volumePowerPost = 0.5;
    volumePowerLast = 0.5;
    volumePowerLastDB = mindb;
    volumePowerLastDBScaled = 0.5;
    volumePowerMaxDBScaled = 0.5;
    volumePowerMaxTimer = 1;
    volumePowerSmoothedDBScaled = 0.5;
    volumePowerAutoGainDB = 0;
    volumePowerAutoGainDBScaled = 0.5;
    volumePowerAutoGainFactor = 0.5;
    volumePowerClipCount = 0;
}


    void ldSoundData::volumeCorrectionApply()
{
    
    // get current frame's power
    float currentpower = 0;

    //freq based version
    //for (int i = 1; i < m_frequency.GetSize()-1; i++) {
    //    currentpower += m_frequency[i];
    //}
    //currentpower /= 6500; // hard coded calibration, 1 = death magnetic

    //max pcm value version
    //for (int i = 0; i < m_waveform.GetSize(); i++) {
    //    float f;
    //    f = fabsf(m_waveform.GetLeft()[i]);
    //    if (f > currentpower) currentpower = f;
    //    f = fabsf(m_waveform.GetRight()[i]);
    //    if (f > currentpower) currentpower = f;
    //}
    
    //rms version
    currentpower = 0;
    int size =  m_waveform.GetSize();
    for (int i = 0; i < size; i++) {
        float m = 1;// ((float)(size - i)) / size;
        float f;
        f = fabsf(m_waveform.GetLeft()[i]);
        f = f * f * m;
        currentpower += f;
        f = fabsf(m_waveform.GetRight()[i]);
        f = f * f * m;
        currentpower += f;
    }
    currentpower /= size*2;
    currentpower = sqrtf(currentpower);
    if (size == 0) currentpower = 0;

    volumePowerPre = currentpower;
    if (currentpower <= 0) currentpower = mindb; else currentpower = 10 * (logf(currentpower) / logf(10));
    if (currentpower<mindb) currentpower = mindb;
    if (currentpower>maxdb) currentpower = maxdb;
    float volumePowerPreDB = currentpower;

    currentpower = volumePowerLast;
    volumePowerLast = volumePowerPre;
    if (currentpower <= 0) currentpower = mindb; else currentpower = 10*(logf(currentpower)/logf(10));    
    if (currentpower<mindb) currentpower = mindb;
    if (currentpower>maxdb) currentpower = maxdb;    
    volumePowerLastDB = currentpower;
    volumePowerLastDBScaled = (currentpower - mindb) / (maxdb - mindb);


    //for (int i = frames-1; i > 0; i--) {
    //    power[i] = power[i-1];
    //}
    //power[0] = (currentpower - mindb) / (maxdb - mindb);
    //{float &t = power[0]; if (t<mindb)t=mindb; if (t>maxdb)t=maxdb;}
    // average over history    
    //float sum = 0, total = 0;
    //static int tframes = frames/10; if (tframes<frames)tframes++;
    //for (int i = 1; i < tframes; i++) {
        //float f = 1 - (float(i)/tframes);
        //sum += power[i]*f;
        //total += f;
    //}
    //float averagepower = sum/total;
    //if (averagepower < mindb) averagepower = mindb;
    //if (averagepower > maxdb) averagepower = maxdb;    
    // max value version
    //float maxpower = 0;
    //float nframes = tframes/1;
    //for (int i = 1; i < nframes; i++) {
    //float f = (float(i-1) / nframes);
        //f = sinf((f+1)*M_PI / 2);
        //float tpower = power[i] * f;
        //qDebug() << i << " " <<  f;
        //if (tpower > maxpower) maxpower = tpower;
    //}

    
    float &current = volumePowerLastDBScaled;
    float &maxpower = volumePowerMaxDBScaled;
    float &mpto = volumePowerMaxTimer;    
    mpto += AUDIO_UPDATE_DELTA_S / falltime;
    if (mpto > 1) mpto = 1;
    maxpower += AUDIO_UPDATE_DELTA_S / falltime * cosf((mpto+1)*M_PI/2);
    if (maxpower < 0) maxpower = 0;
    if (volumePowerLastDBScaled >= maxpower) {
        maxpower = current;
        mpto = 0;    
    }
    
    float &ap = volumePowerSmoothedDBScaled;    
    float dd = volumePowerMaxDBScaled - ap;
    float mx = AUDIO_UPDATE_DELTA_S / risetime / 2;
    if (dd > 0.10) mx *= 1 + qMin<float>(1, (dd - 0.10) / (0.40))*4;
    if (dd > 1 * mx) {
        //dd = 1*mx + qMin<float>(1*mx, (dd-1*mx)/4); 
        dd = 1 * mx;
        //if (dd > 0.25) dd += 1 * mx * qMin<float>(1, (dd - 0.25)*2);
    }
    //if (dd > 1*mx) dd = 1*mx;
    //if (dd < -2*mx) dd = -2*mx;
    ap += dd;
    if (ap < 0) ap = 0;
    if (ap > 1) ap = 1;
        
    float desiredpower = targetdb;
    float averagepower = mindb + (maxdb - mindb)*ap;
    float &dbamp = volumePowerAutoGainDB;
    dbamp = (desiredpower-averagepower);    

    // don't allow a frame to be amplified above ceildb
    bool clipped = false;
    float expectedpower = volumePowerPreDB + dbamp;
    if (expectedpower > ceildb) {
        dbamp = ceildb - volumePowerPreDB;
        clipped = true;
    }    
    volumePowerAutoGainDBScaled = (dbamp - (targetdb-maxdb)) / ((targetdb-mindb) - (targetdb-maxdb));


    float s = 1*powf(10, dbamp/10);
    volumePowerAutoGainFactor = s;    

    // apply
    float* left = m_waveform.GetLeft();
    float* right = m_waveform.GetRight();

    for (uint i = 0; i < m_waveform.GetSize(); i++) {
        left[i] *= s;
        right[i] *= s;
        if (left[i]  < -1) left[i] =  -1;
        if (right[i] < -1) right[i] = -1;
        if (left[i]  >  1) left[i] =   1;
        if (right[i] >  1) right[i] =  1;
        if (left[i] * left[i] > 0.999 || right[i] * right[i] > 0.999) {            
            clipped = true;
        }       
    }
    if (clipped) {
        volumePowerClipCount++;
    }

    for (uint i = 0; i < m_frequency.GetSize(); i++) {
        m_frequency[i] *= s;
    }

    _averageL *= s;
    _averageR *= s;
    _peakL *= s;
    _peakR *= s;
    volumePowerPost = volumePowerPre * s;

}

// not supported
//void ldSoundData::GetLevels(float *left, float *right) {
    // Average & peak are stored squared leaving sqrtf to JIT
//    *left  = _averageL;
//   *right = _averageR;
//}

// not supported
//void ldSoundData::GetLevels(float *buffer) {
    // Average & peak are stored squared leaving sqrtf to JIT
    //buffer[0] = _averageL; buffer[1] = _peakL;
    //buffer[2] = _averageR; buffer[3] = _peakR;
//}

static float aw(float f) {
    float r = 1;
    if (f < 110) r = (f / 110);
    if (f > 880) r = 1 - ((f - 880)/880/10);
    r *= 1.27f;
    if (r < 0) r = 0;
    if (r > 1) r = 1;
    return r;
}

// GetFFTInFreqRange (Alec)
float ldSoundData::GetFFTInFreqRange(float startHz, float endHz, bool normalize, bool weight, bool max) const {
    
    int s = startHz / 30;
    int e = endHz / 30;
    
    s = qMax<int>(s, 1);
    s = qMin<int>(s, 735);

    e = qMax<int>(e, s + 1);
    e = qMax<int>(e, 1);
    e = qMin<int>(e, 735);
        
    float sum = 0;
    float total = 0;
    float sup = 0;
    for (int i = s; i < e; i++) {
        float v = m_frequency[i];
        if (normalize) v *= (i+1);
        if (weight) v *= (aw(i * 30));
        sum += v;
        sup = qMax<float>(sup, v);
        total++;
    }
    if (total == 0) return 0;
    float r = sum / total;
    if (max) r = sup;
    r *= (1.0f / SAMPLE_SIZE);
    r = qMax<float>(0, r);
    r = qMin<float>(1, r);    
    return r;
}

// GetFullFFT (Eric based on Alec's GetFFTInFreqRange)
void ldSoundData::GetFullFFT(std::vector<float>& fftArray, bool normalize, bool weight) const
{
    // fftArray needs to be 0/734 to get 1/735 bin values
    if (fftArray.size() != SAMPLE_SIZE/2) return;
    //
    int s = 1;
    int e = SAMPLE_SIZE/2;
//    float sum = 0;
//    float total = 0;
//    float sup = 0;
    float max = 0;
    std::vector<float> tmp = std::vector<float>();
    tmp.resize(fftArray.size());
    for (int i = s; i < e; i++) {
        float v = m_frequency[i];
        if (normalize) v *= (i+1);
        if (weight) v *= (aw(i * 30));
        tmp[i] = v;
        max = qMax<float>(max, v);
    }
    //qDebug() << "max" << max;
    if (fabs(max) < 0.0000001) return;
    for (int i = s; i < e; i++) {
        float r = tmp[i]/max;
        r = qMax<float>(0, r);
        r = qMin<float>(1, r);
        fftArray[i] = r;
        //if ( i < 10) qDebug() << "fftArray[i]"<< fftArray[i] << "v/max" << v/max;
    }
}

float ldSoundData::GetFFTValueForFrequency(float frequencyInHertz, unsigned windowSize) const {
    if (frequencyInHertz < 0 || frequencyInHertz >= soundMaxFrequency) return 0;
    int fIdx = floorf((frequencyInHertz / (soundMaxFrequency * 2.0)) * SAMPLE_SIZE);
    if (fIdx > SAMPLE_SIZE / 2) fIdx = SAMPLE_SIZE / 2;
    float scale = 1.0f /  (float)SAMPLE_SIZE;
    float value = 0.0;
    if (windowSize == 0)
        value = m_frequency[fIdx] * scale;
    else {
        unsigned firstIdx = fIdx - windowSize;
        unsigned lastIdx =  fminf(SAMPLE_SIZE/2, fIdx + windowSize);
        for (unsigned idx = firstIdx; idx < lastIdx; idx++)
            value += m_frequency[fIdx] *scale; // scaled
        value *= 1.0/(float)(lastIdx - firstIdx); // averaged
    }
    return value;
}

void ldSoundData::GetSpectrum(float *spectrum, unsigned spectrumLength, float minHz, float maxHz) const {
    maxHz = fminf(soundMaxFrequency, maxHz);// limit to Nyquist
    minHz = fmaxf(0, minHz);                // limit to positive
    float totalBins = (float)SAMPLE_SIZE;   // We have 2940 bins in total
    float scale = 1 /  totalBins;         // KISS_FFT scales results by the sample size in floating-point mode
    // Nyquist limits usable values to first half of FFT values thus Bins 0 - 1470 will have data for freqs 0 - 22050Hz
    // BUT we need to use the total Hz range to calculate the actual frequency range per bin
    float  hzPerBin = (soundMaxFrequency * 2.0) / (float)totalBins;
    unsigned firstBinIndex = floorf(minHz / hzPerBin);
    unsigned lastBinIndex = floorf(maxHz / hzPerBin);
    unsigned numBins = lastBinIndex - firstBinIndex;
    unsigned binsPerBlock = floorf(numBins / spectrumLength);    
    for (unsigned i = 0; i < spectrumLength; ++i) {
        float value = 0.0;                          // buffer
        lastBinIndex = firstBinIndex + binsPerBlock;        
        for (unsigned fIdx = firstBinIndex; fIdx < lastBinIndex; fIdx++)
            value += m_frequency[fIdx] *scale;      // accumulate scaled
        spectrum[i] = value/(float)binsPerBlock;    // store averaged;
        firstBinIndex += binsPerBlock;              // increment to next block
    }
}

int ldSoundData::GetSoundLevel() const
{
    int value = 100.0f * powf(volumePowerPost, 0.5);
    return std::min(std::max(value, 0), 100);
}

float ldSoundData::GetFFTValueForBlock(unsigned blockIndex, unsigned numBlocks) const {
    float totalBins = (float)SAMPLE_SIZE;
    float scale = 1.0 /  totalBins;
    // We have 2940 bins in total, but Nyquist limits usable data range to first half of values
    // Bins 0 - 1470 will have data for freqs 0 - 22kHz
    // Bins 0 - 735  will have data for freqs 0 - 11kHz
    // Bins 0 - 367  will have data for freqs 0 - 5.5kHz
    // Bins 0 - 182  will have data for freqs 0 - 2.25kHz
    // Bins 0 - 91   will have data for freqs 0 - 1.125kHz
    // Bins 0 - 45   will have data for freqs 0 - 550Hz
    // Bins 0 - 22   will have data for freqs 0 - 225Hz
    // Bins 0 - 11   will have data for freqs 0 - 115Hz
    unsigned firstBinIndex = 10;                            // start from about 100 Hz
    float numBins = floorf(totalBins / 8.0);                // go up to around 5000 Hz
    unsigned binsPerBlock = floorf(numBins / numBlocks);    // find num bins per block
    firstBinIndex += blockIndex * binsPerBlock;             // move to first bin in block
    unsigned lastBinIndex = firstBinIndex + binsPerBlock;   // range of bins to sample
    float value = 0.0;                                      // buffer
    for (unsigned fIdx = firstBinIndex; fIdx < lastBinIndex; fIdx++)
        value += m_frequency[fIdx] *scale;                  // accumulate normalized into buffer
    value *= 1.0/(float)binsPerBlock;                       // average
//    value *= 2.0; // Conjecture: As we are ignoring half the data should we scale by 2.0?.
    return value;
}


float ldSoundData::GetWaveformL() const
{
    return _peakL; // only returns the first value per frame
}

float ldSoundData::GetWaveformL(int seek) const
{
    float value = m_waveform.GetLeft()[seek];
    value = fminf(fmaxf(value, -8.0f), 8.0f) / 8.0f;
    // MH Notes: Why is this range-limited to -8..8, then divided by 8?
    // Waveform data is already in the range -1..1
    // Returns values in the range -0.2222215..0.2222215
    return value;
}

float ldSoundData::GetWaveformR() const
{
    return _peakR; // only returns the first value per frame
}

float ldSoundData::GetWaveformR(int seek) const
{
    float value = m_waveform.GetRight()[seek];

    value = fminf(fmaxf(value, -8.0f), 8.0f) / 8.0f;
    // MH Notes: Why is this range-limited to -8..8, then divided by 8?
    // Waveform data is already in the range -1..1
    // Returns values in the range - 0.2222215..0.2222215
    return value;
}

// MH Notes:
// The main limitation of the methods below is that they rely on powerAt() which then
// uses GetFFTValue() to retrieve only a simgle frequency from the 1470 bins available...
// in practice, with a division-size of 20, each frequency 'band' covers 74 bins, and from those
// bins we are only grabbing the first bins value, and ignoring the other 73 bins, some of which
// may contain more relevant data for the desired frequency range.

// GetFFTValue() also has some problems in that it scales the fft values to the range 0..197 then
// clips to 0..8 before scaling again by 1/8.0.

// The compromise here is that in only using the frequencies up to ~3000 Hz, we are only using
// the first 200 or so bins, so in practice these methods are using ~1-in-10 samples, ie discarding ~9/10ths of data
// rather than discarding 73/74 ths of data in the worst-case.

// A better solution for smoother spectral data is to average the values from adjoining bins as is done in the
// new GetMusicSpectrum(), GetSpectrum(), and GetFFTValueForBlock() methods.


float ldSoundData::GetBass() const
{
    float value = powerAt(0, 20) + powerAt(1, 20) + powerAt(2, 20)
    + powerAt(3, 20) + powerAt(4, 20) + powerAt(5, 20);

    return value / 6.0f;
}

float ldSoundData::GetMids() const
{
    float value = powerAt(6, 20) + powerAt(7, 20) + powerAt(8, 20)
    + powerAt(9, 20) + powerAt(10, 20) + powerAt(11, 20)
    + powerAt(12, 20) + powerAt(13, 20) + powerAt(14, 20);

    return value / 9.0f * 2;
}

float ldSoundData::GetHigh() const
{
    float value = powerAt(15, 20) + powerAt(16, 20) + powerAt(17, 20)
    + powerAt(18, 20) + powerAt(19, 20);

    return value / 5.0f * 3;
}

float ldSoundData::GetFFTValue(int frequency) const
{
    if (frequency < 0 || frequency >= soundMaxFrequency) return 0;

    int fIdx = frequency / frequencyGap;

    if (fIdx >= SAMPLE_SIZE / 2 || fIdx < 0) return 0;

    float value = m_frequency[fIdx];

    // adjustment the value between 0 and 1
    value = fminf(fmaxf(value / 15.0f, 0.0f), 8.0f) / 8.0f;

    // MH Notes:
    // KISS FFT in floating point mode returns fft values that are scaled by fft-size, i.e. SAMPLE_SIZE
    // Therefore the correct scaling to get values in a 0..1 range is to divide returned values by SAMPLE_SIZE.

    // Given our SAMPLE_SIZE of 2940, the method used here first scalues values to the range 0..197,
    // then clips the values to a max of 8.0, before dividing by 8.0 to scale the clipped values to the 0..1 range.

    // This is potentially highly innaccurate but as many existing visualizers have been tweaked to respond to values
    // with this scaling & clipping I have left it unchanged... I would suggest new visualizers use the newer
    // GetMusicSpectrum(), GetSpectrum(), and GetFFTValueForBlock() which all return un-clipped values on the 0..1 range

    return value;
}

float ldSoundData::powerAt(int index, int spectrumLength) const
{

    // we use only the frequency range from 0 to 3000Hz.
    //float frequency = USING_MAX_FREQ * i * 10.0f / spectrumLength;

    // MH Notes: As USING_MAX_FREQ is 369.9, we MUST multiply by 10 to get upper frq of 3699 Hz

    // Given that USING_MAX_FREQ is defined as 369.99, I can't see how this method
    // ever previously produced a target frequency higher than 369.99 Hz
    // Possibly this was missed due to the previous incorrect loading of data into FFT buffers causing garbage output

    float maxHz = 22050; //fminf(soundMaxFrequency, 3000);// limit to Nyquist
    //qDebug()<< soundMaxFrequency << ", " << m_format.sampleRate();
    float minHz = 1; //fmaxf(0, 100);                // limit to positive
    float totalBins = (float)SAMPLE_SIZE;   // We have 2940 bins in total
    float scale = 1.0 /  totalBins;         // KISS_FFT scales results by the sample size in floating-point mode
    // Nyquist limits usable values to first half of FFT values thus Bins 0 - 1470 will have data for freqs 0 - 22050Hz
    // BUT we need to use the total Hz range to calculate the actual frequency range per bin

    // feldspar note:  corrected to use sample rate instead of nyquist freq as described
    //float  hzPerBin = (maxHz - minHz) / (float)totalBins; // old version
    float  hzPerBin = (44100 - minHz) / (float)totalBins;

    unsigned firstBinIndex = floorf(minHz / hzPerBin);
    unsigned lastBinIndex = floorf(maxHz / hzPerBin);
    unsigned numBins = lastBinIndex - firstBinIndex;
    unsigned binsPerBlock = floorf(numBins / spectrumLength);

    //for (unsigned i = 0; i < (unsigned)spectrumLength; ++i)
    unsigned i = index;
    firstBinIndex += binsPerBlock*i;
    lastBinIndex = firstBinIndex + binsPerBlock;     
    {
        float value = 0.0;                          // buffer
       // lastBinIndex = firstBinIndex + binsPerBlock;

        for (unsigned fIdx = firstBinIndex; fIdx < lastBinIndex; fIdx++)
        {
            //FIXME follows causes program to crash after restarting visualizer
            value += m_frequency[fIdx] * scale * (fIdx + 1)*3;// / 735 * 8;      // accumulate scaled
        }

        if (i == (unsigned)index)
        {
            value = value/(float)binsPerBlock;
            //value *= 200; // adjusting ADVERAGE-FFT VALUE INTO 0..1
            return fminf(fmaxf(value, 0.0f), 1.0f);
        }
        //spectrum[i] = value/(float)binsPerBlock;    // store averaged;
        //firstBinIndex += binsPerBlock;              // increment to next block
    }

    return 0;

    //return GetFFTValue(frequency);
}
