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
class LDCORESHARED_EXPORT ldDurationalStatEstimator {
public:
    //static const int maxArraySize = 3*30*AUDIO_OVERDRIVE_FACTOR;
    //float historyValues[maxArraySize];
    //int historySize;
    // use cheap estimator algorithm (stateless iir)

    ldDurationalStatEstimator(float seconds = 3);
    void add(float f);

    float mean;
    float variance;
    float stdDev;
    float variation;

    float rho;
};

class LDCORESHARED_EXPORT ldPitchTracker {
public:
    static const int n24 = 6;
    ldDurationalStatEstimator chromas[n24];
    float s[n24] = {};
    float maxChroma;
    float confidence;
    ldPitchTracker();
    void update(ldSoundData* psd, float factor, bool transition, float _freq);

    float freq = 0.f;
    float progress = 0.f;
    float output = 0.f;
};

class LDCORESHARED_EXPORT ldSpectAdvanced {
public:
    ldSpectAdvanced(float rho = 0.707);
    void update(const ldSpectrumFrame &f);
    static const int slsLen = ldSpectrumFrame::slsLen;
    ldDurationalStatEstimator dse[slsLen];
    float value[slsLen];
    float average[slsLen];
    float normalized[slsLen];
    float peak;
    float mean;
    ldDurationalStatEstimator dsep2, dsep3;
    ldDurationalStatEstimator dsem2, dsem3;
    float peak2, peak3;
    float mean2, mean3;

};

class LDCORESHARED_EXPORT ldSigmoidDetector {
public:
    ldSigmoidDetector();
    void update(float f, float bpf);
    static const int maxlen = 256;
    float hist[maxlen];
    float output;
};


class LDCORESHARED_EXPORT ldBeatWarm {
public:
    ldSigmoidDetector sd[24];
    ldBeatWarm();
    void update(ldSpectAdvanced* spectAdvanced, ldSpectAdvanced* spectAdvanced2, float freq1, float freq2);
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

class LDCORESHARED_EXPORT ldBeatFresh {
public:
    static const int len = 5;
    static const int histSize = 15*30 * AUDIO_OVERDRIVE_FACTOR;
    float hist[histSize][len];
    ldBeatFresh();
    float update(const std::vector<float> vect, int sf);
    ldDurationalStatEstimator n;
    ldDurationalStatEstimator o;
    float output = 0.f;
    float output2 = 0.f;
    float outputNorm = 0.f;
    float outputVar = 0.f;
};

#endif // LDDURATIONALSTATESTIMATOR_H
