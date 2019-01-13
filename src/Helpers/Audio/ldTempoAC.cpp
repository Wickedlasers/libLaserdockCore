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

#include "ldCore/Helpers/Audio/ldTempoAC.h"
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"
#include <QtCore/QDebug>

ldTempoAC::ldTempoAC()
{
    offsetPower.resize(histLen);
    offsetPower2.resize(histLen);

    clear();

    preCalcValues();
}


ldTempoAC::ldTempoAC(float _targetTempoBPM, float _fadeHalfLife, float _trebleBias, bool _alg)
{
    offsetPower.resize(histLen);
    offsetPower2.resize(histLen);

    m_targetTempoBpm = _targetTempoBPM;
    m_fadeHalfLife = _fadeHalfLife;
    m_trebleBias = _trebleBias;
    m_alg = _alg;

    clear();

    preCalcValues();
}


void ldTempoAC::update(ldSpectrogram* spectrogram, bool ismusic)
{
    if (!ismusic) {
        clear();
        return;
    }

    calcOffsetPower(spectrogram);

    // determine instant bpm
    int bestt = 4;
    float bestv = 0;

    float sum = 0, total = 0;
    for (int t = 4; t < (histLen-2)/2-2; t++) { // 4 <= t < 357
        float et = envelope(t);

        float p = powf(offsetPower[t]*et, 5);

        if (p > bestv) {
            bestv = p;
            bestt = t;
        }

        // remember average

        sum += p*et;
        total += et;
    }

    float cav = sum/total;
    confidence = bestv / (cav+bestv);
    confidence = sqrt(confidence-0.5)*3;
    clampfp(confidence, 0, 1);

    bpmInstant = 60/(bestt*AUDIO_UPDATE_DELTA_S);
    wavelenInstant = bestt;

    int bestp = calcBestP(spectrogram, bestt);
    float pp = float(bestp)/bestt;
    phaseInstant = 1-pp;

    // walker

    float h = 1.0/(wavelenInstant-0.0);
    phaseWalker -= h;
    float tsmooth = (1.0/freqSmooth);// - 0.825;
    if (tsmooth >= 0) phaseSmooth -= 1.0/tsmooth; else phaseSmooth = 0;
    phaseReactive -= freqSmooth;
    if (phaseSmooth < 0) phaseSmooth = 0;
    if (phaseReactive < 0) phaseReactive = 0;

    float d = (phaseInstant) - phaseWalker;
    if (d > 0.5) d--;
    if (d < -0.5) d++;
    if (d < -h*0.0) d = -h*0.0;
    if (d > h*2.0) d = h*2.0;
    phaseWalker += d;
    if (phaseWalker > 1) phaseWalker = 1;
    if (phaseWalker < 0) phaseWalker++;

    if (phaseSmooth <= h*2) {
        if (phaseInstant > (0.5-h*2)) {
            phaseSmooth = 1;
            freqSmooth = h;
        }
        if (phaseWalker > (1-h*1.5)) {
            phaseSmooth = 1;
            freqSmooth = h;
        }
    }

    if (phaseInstant >= (1-h*2)) phaseReactive = phaseInstant;

    bpmSmooth = freqSmooth / AUDIO_UPDATE_DELTA_S * 60.0f;
}

void ldTempoAC::clear() {
    bpmInstant = bpmSmooth = m_targetTempoBpm;
    wavelenInstant = 1/bpmInstant;
    freqSmooth = bpmSmooth * AUDIO_UPDATE_DELTA_S / 60.0f;
    phaseInstant = phaseWalker = phaseReactive = phaseSmooth = 0;
    confidence = 0;
    std::fill(offsetPower.begin(), offsetPower.end(), 0.f);
    std::fill(offsetPower2.begin(), offsetPower2.end(), 0.f);
}

float ldTempoAC::envelope(float t) {
    // faster
    {
        float m = (60.0 / m_targetTempoBpm) / AUDIO_UPDATE_DELTA_S;
        if (t > m * 1.5) return 0;
        if (t < m / 1.5) return 0;
        return 1;
    }

    // fastest
    return 1;//

    // normal
    float midt = (60.0/m_targetTempoBpm) / AUDIO_UPDATE_DELTA_S;
    float widt = midt/2;//0.5 / AUDIO_UPDATE_DELTA_S;
    float e = (t-midt)/widt;
    e = powf(e, 8);
    e = 1 - e;
    e = fmin(fmax(0, e), 1);

    e = t/midt;
    if (e < 1) e = 1/e;
    e = log(e)/log(2);

    if (e >= 1) return 0;
    if (e <= 0) return 1;

    e = (1+cos(e*3.141592653589793238462643383251))/2;

    e = fmin(fmax(0, e), 1);
    return e;
}

void ldTempoAC::preCalcValues()
{
    m_m = powf(0.5f, AUDIO_UPDATE_DELTA_S/m_fadeHalfLife);//0.99;
    m_n = 1-m_m;
}

int ldTempoAC::calcBestP(ldSpectrogram* spectrogram, int bestt)
{
    int bestp = 0;
    float bestpp = 0;
    for (int phase = 0; phase < bestt; phase++) {
        float best_sum = 0, best_total = 0;
        best_sum += spectrogram->frame(phase+bestt*0+0).getEnergyScaled(m_trebleBias)*0.999/1;
        best_sum += spectrogram->frame(phase+bestt*1+0).getEnergyScaled(m_trebleBias)*0.666/1;
        best_sum += spectrogram->frame(phase+bestt*2-1).getEnergyScaled(m_trebleBias)*0.333/3;
        best_sum += spectrogram->frame(phase+bestt*2+0).getEnergyScaled(m_trebleBias)*0.333/3;
        best_sum += spectrogram->frame(phase+bestt*2+1).getEnergyScaled(m_trebleBias)*0.333/3;
        best_total += 2;//2.111;
        float p = best_sum / best_total;

        if (m_alg) {
            qDebug() << "alg0=" << p;
            const ldSpectrumFrame* p1 = &spectrogram->frame(phase+bestt*0);
            const ldSpectrumFrame* p2 = &spectrogram->frame(phase+bestt*1);
            p = ldSpectrumFrame::compare(p1, p2);
            qDebug() << "alg1=" << p;
        }

        if (p > bestpp) {
            bestpp = p;
            bestp = phase;
        }
    }

    return bestp;
}

void ldTempoAC::calcOffsetPower(ldSpectrogram* spectrogram)
{
    for (int t = 1; t < ldSpectrogram::histLen; t++) {
        float s = spectrogram->getS(t);
        offsetPower[t] = offsetPower[t] * m_m + m_n * s;
    }
}

