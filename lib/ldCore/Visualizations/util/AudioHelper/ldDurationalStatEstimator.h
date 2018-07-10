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

#ifndef LDDURATIONALSTATESTIMATOR_H
#define LDDURATIONALSTATESTIMATOR_H

#include <ldCore/Sound/ldSoundData.h>

#include "ldSpectrumFrame.h"

// averages a signal over time
class LDCORESHARED_EXPORT DurationalStatEstimator {
public:
    //static const int maxArraySize = 3*30*AUDIO_OVERDRIVE_FACTOR;
    //float historyValues[maxArraySize];
    //int historySize;
    // use cheap estimator algorithm (stateless iir)

    DurationalStatEstimator(float seconds = 3);
    void add(float f);

    float mean;
    float variance;
    float stdDev;
    float variation;

    float rho;
};

class LDCORESHARED_EXPORT PitchTracker {
public:
    static const int n24 = 6;
    DurationalStatEstimator chromas[n24];
    float s[n24] = {};
    float maxChroma;
    float confidence;
    PitchTracker();
    void update(ldSoundData* psd, float factor, bool transition, float _freq);

    float freq = 0.f;
    float progress = 0.f;
    float output = 0.f;
};

class LDCORESHARED_EXPORT SpectAdvanced {
public:
    SpectAdvanced(float rho = 0.707);
    void update(const ldSpectrumFrame &f);
    static const int slsLen = ldSpectrumFrame::slsLen;
    DurationalStatEstimator dse[slsLen];
    float value[slsLen];
    float average[slsLen];
    float normalized[slsLen];
    float peak;
    float mean;
    DurationalStatEstimator dsep2, dsep3;
    DurationalStatEstimator dsem2, dsem3;
    float peak2, peak3;
    float mean2, mean3;

};

class LDCORESHARED_EXPORT SigmoidDetector {
public:
    SigmoidDetector();
    void update(float f, float bpf);
    static const int maxlen = 256;
    float hist[maxlen];
    float output;
};


class LDCORESHARED_EXPORT BeatWarm {
public:
    SigmoidDetector sd[24];
    BeatWarm();
    void update(SpectAdvanced* spectAdvanced, SpectAdvanced* spectAdvanced2, float freq1, float freq2);
    float output = 0.f;
};

/*
class DropDetector {
public:
    DropDetector();
    SpectAdvanced* sa1;
    SpectAdvanced* sa2;
    void update(SpectrumFrame& f);
    static const int nInputs = 8;
    float input[nInputs];
    SigmoidDetector sd[nInputs];
    float output[nInputs];
};
*/

class LDCORESHARED_EXPORT BeatFresh {
public:
    static const int len = 5;
    static const int histSize = 15*30 * AUDIO_OVERDRIVE_FACTOR;
    float hist[histSize][len];
    BeatFresh();
    float update(const std::vector<float> vect, int sf);
    DurationalStatEstimator n;
    DurationalStatEstimator o;
    float output = 0.f;
    float output2 = 0.f;
    float outputNorm = 0.f;
    float outputVar = 0.f;
};

#endif // LDDURATIONALSTATESTIMATOR_H
