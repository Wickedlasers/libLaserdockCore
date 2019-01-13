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

#include "ldCore/Helpers/Audio/ldDurationalStatEstimator.h"

#include <math.h>
#include <ldCore/Sound/ldSoundData.h>
#include <QtCore/QDebug>
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

ldDurationalStatEstimator::ldDurationalStatEstimator(float /*seconds*/) {
    //historySize = seconds*30*AUDIO_OVERDRIVE_FACTOR;
    //for (int i = 0)
    mean = 0;
    variance = 0;
    stdDev = 0;
    variation = 0;
    rho = 0.707f;
}
void ldDurationalStatEstimator::add(float f) {

    float z = powf(rho, 1.0f/(30*AUDIO_OVERDRIVE_FACTOR));
    float cz = 1-z;

    mean = z*mean + cz*f;

    float vf = (f-mean)*(f-mean);
    variance = z*variance + cz*vf;

    stdDev = sqrt(variance);
    variation = 0;
    if (mean > 0) variation = stdDev / mean;

    clampfp(mean, 0, 1);
    clampfp(stdDev, 0, 10);
    clampfp(variance, 0, 100);
    clampfp(variation, 0, 1000);

}

//DurationalStatEstimator chromas[24];
//float maxChroma;
ldPitchTracker::ldPitchTracker() {
    maxChroma = 0;
    confidence = 0;
}
void ldPitchTracker::update(ldSoundData* psd, float factor, bool transition, float _freq) {

    //int n24 = 6;
    for (int i = 0; i < n24; i++) {
        float v = 0;
        float f1 = powf(2, 1+(i+0.5)/20.0);
        float f2 = powf(2, 1+(i+1.5)/20.0);
        float sum = 0, total = 0;
        float ft1, ft2;
        for (ft1 = f1*2, ft2 = f2*2; ft2 < SAMPLE_SIZE/2; ft1 += f1, ft2 += f2) {
            float t = 0;
            int ft1i = ft1;
            int ft2i = ft2;
            for (int fti = ft1i; fti <= ft2i; fti++) {
                t += psd->GetFFTValueForFrequency(fti*30) / (ft2i - ft1i + 1);
            }
            float m = (ft1+ft2)/2;
            sum += t * m;
            total += m;
        }
        /*for (int j = 1; i < SAMPLE_SIZE/2; j++) {

            }*/
        v = sum / total;
        chromas[i].rho = 0.707f / 2;
        chromas[i].rho = 0.101f;
        chromas[i].add(v*factor);
    }

    float sum2 = 0;
    for (int i = 0; i < n24; i++) {
        float f = 0;
        f += chromas[(i - 1 + n24)%n24].mean;
        f += chromas[(i     + n24)%n24].mean;
        f += chromas[(i + 1 + n24)%n24].mean;
        s[i] = f;
        sum2 += f;
    }
    for (int i = 0; i < n24; i++) s[i] /= sum2;

    int maxi = 0;
    float sum = 0, total = 0;
    for (int i = 0; i < n24; i++) {
        if (s[i] > s[maxi]) maxi = i;
        sum += s[i];
        total += 1;
    }
    maxChroma = (float) maxi/n24;

    //float n = maxChroma;
    //output = progress*n + (1-progress)*o;
    if (transition && progress == 0) {
        freq = _freq;
        if (progress == 0) progress = freq;
    }
    if (progress > 0) {
        progress += freq;
        float delta = maxChroma - output;
        if (delta < -0.5) delta++;
        if (delta >  0.5) delta--;
        output += delta*progress*freq*2;
    }
    if (progress >= 1) {
        output = maxChroma;
        progress = 0;
    }

    confidence = n24 * s[maxi] / sum;
    //confidence = n24 * s[int(output*23.99)] / sum;

}


ldSpectAdvanced::ldSpectAdvanced(float rho) {
    for (int i = 0; i < slsLen; i++) {
        dse[i].rho = rho;
        value[i] = 0;
        average[i] = 0;
        normalized[i] = 0;
    }
    peak = 0;
    mean = 0;
    float rho2 = sqrtf(rho);
    dsep2.rho = dsep3.rho = rho2; peak2 = peak3 = 0;
    dsem2.rho = dsem3.rho = rho2; mean2 = mean3 = 0;
}

void ldSpectAdvanced::update(const ldSpectrumFrame &f) {

    float sum = 0, total = 0;
    float max = 0;
    float factors[slsLen] = {0.66f,1.00f,1.00f,1.00f,1.00f,0.75f,0.50f,0.00f};

    for (int i = 0; i < slsLen; i++) {
        dse[i].add(f.sls[i]);
        value[i] = f.sls[i];
        average[i] = dse[i].mean;
        normalized[i] = (average[i] > 0) ? (value[i]/average[i]) : (0);
        normalized[i] *= factors[i];
        if (normalized[i] > max) max = normalized[i];
        sum += normalized[i]; total += factors[i];
    }

    peak = max;
    float tpeak = logf(peak)/logf(2); if (tpeak < 1) tpeak = 1;
    dsep2.add(tpeak);
    peak2 = tpeak / dsep2.mean;
    dsep3.add(peak);
    peak3 = (peak-dsep3.mean)/dsep3.stdDev;

    mean = sum / total;
    float tmean = logf(mean)/logf(2); if (tmean < 1) tmean = 1;
    dsem2.add(tmean);
    mean2 = tmean / dsem2.mean;
    dsem3.add(mean);
    mean3 = (mean-dsem3.mean)/dsem3.stdDev;

}

ldSigmoidDetector::ldSigmoidDetector() {
    for (int i = 0; i < maxlen; i++) {
        hist[i] = 0;
    }
    output = 0;
}

void ldSigmoidDetector::update(float f, float bpf) {
    for (int i = maxlen-1; i >= 1; i--) hist[i] = hist[i-1];
    hist[0] = f;
    int slen = 1/bpf;
    //qDebug() << slen;
    if (slen > maxlen) slen = maxlen;
    float total = 0;
    for (int i = 0; i < slen; i++) {
        float fi = float(i)/float(slen-1);
        float m = rcost(fi/2)-0.5;
        total += hist[i] * m;
    }
    output = total / slen * 4;
}

ldBeatWarm::ldBeatWarm() {

}
//SigmoidDetector sd[24];
void ldBeatWarm::update(ldSpectAdvanced* spectAdvanced, ldSpectAdvanced* spectAdvanced2, float freq1, float freq2) {

    int ii = 0;    float dd[24];
    dd[ii++] = (spectAdvanced->peak / 20);
    dd[ii++] = (spectAdvanced->peak2 / 10);
    dd[ii++] = (spectAdvanced->peak3 / 10);

    dd[ii++] = (spectAdvanced->mean / 10);
    dd[ii++] = (spectAdvanced->mean2 / 10);
    dd[ii++] = (spectAdvanced->mean3 / 10);

    dd[ii++] = (spectAdvanced2->peak / 20);
    dd[ii++] = (spectAdvanced2->peak2 / 10);
    dd[ii++] = (spectAdvanced2->peak3 / 10);

    dd[ii++] = (spectAdvanced2->mean / 10);
    dd[ii++] = (spectAdvanced2->mean2 / 10);
    dd[ii++] = (spectAdvanced2->mean3 / 10);

    for (int i = 0; i < 12; i++) {
        clampfp(dd[i], 0, 1);
        sd[i].update(dd[i], freq1);
        sd[i+12].update(dd[i], freq2);
    }

    /*sd[0].update(spectAdvanced->peak/10, tempoACFast->freqSmooth*2);
    sd[1].update(spectAdvanced->peak3/5, tempoACFast->freqSmooth*2);
    sd[2].update(spectAdvanced->peak/10, tempoACSlow->freqSmooth*2);
    sd[3].update(spectAdvanced->peak3/5, tempoACSlow->freqSmooth*2);
    sd[4].update(spectAdvanced2->peak/10, tempoACFast->freqSmooth*2);
    sd[5].update(spectAdvanced2->peak3/5, tempoACFast->freqSmooth*2);
    sd[6].update(spectAdvanced2->peak/10, tempoACSlow->freqSmooth*2);
    sd[7].update(spectAdvanced2->peak3/5, tempoACSlow->freqSmooth*2);*/


    float onsetBeatWarm = sd[14].output;
    onsetBeatWarm *= 4;
    onsetBeatWarm = powf(onsetBeatWarm, 1.0f/3.0f);
    output = onsetBeatWarm;

}


/*class BeatFresh:: {
public:
    static int len = 4;
    static int histSize = 120 * AUDIO_OVERDRIVE_FACTOR;
    float hist[histSize][len];
    BeatFresh();
    float update(float vect[len]);
    DurationalStatEstimator n;
};*/


ldBeatFresh::ldBeatFresh() {
    n.rho = 0.85f;
    o.rho = 0.00002f;
    for (int i = 0; i < histSize; i++) {
        for (int j = 0; j < len; j++) {
            hist[i][j] = 0;
        }
    }
}

float ldBeatFresh::update(const std::vector<float> vect, int sf) {
    for (int i = 0; i < len; i++) {
        for (int j = histSize - 1; j >= 1; j--) {
            hist[j][i] = hist[j-1][i];
        }
        hist[0][i] = vect[i];
    }
    float sum = 0;
    float total = 0;
    float min = -1;
    for (int i = 1+sf; i < histSize; i++) {
        float factor = 1 - (float(i)/(histSize-1));
        float norm = 0;
        for (int j = 0; j < len; j++) {
            float d = hist[0][j] - hist[i][j];
            //norm += fabsf(d);
            norm += d*d;
            //norm *= (hist[0][j] * hist[i][j]);
        }
        norm = sqrtf(norm) / sqrt(len);
        //norm = norm*norm;
        //norm = log(1+norm)/log(2);
        float dd = norm;
        if (min == -1 || dd < min) min = dd;
        //sum += factor * norm;
        sum += factor * min;
        total += factor;
    }
    float average = sum / total;
    //average = min;
    //average = powf(2, average);
    n.add(average);
    output = average;
    outputNorm = average / n.mean;
    outputVar = (average - n.mean)/n.stdDev;
    float t = outputVar / 20; if (t < 0) t = 0;
    o.add(t);
    output2 = o.mean;
    return output;
}

