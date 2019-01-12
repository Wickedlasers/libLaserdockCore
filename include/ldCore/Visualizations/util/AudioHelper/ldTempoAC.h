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

#ifndef _LDTEMPOAC_H
#define _LDTEMPOAC_H

#include "ldSpectrogram.h"

class LDCORESHARED_EXPORT ldTempoAC
{
public:
    ldTempoAC();
    ldTempoAC(float _targetTempoBPM, float _fadeHalfLife, float _trebleBias, bool _alg);

    void update(ldSpectrogram* spectrogram, bool ismusic = true);

    // rhythm phazors
    int wavelenInstant = 0;
    float bpmInstant = 0.f;
    float bpmSmooth = 0.f;
    float freqSmooth = 0.f; // bpmSmooth converted to units of (frames^-1)
    float phaseInstant = 0.f;
    float phaseWalker = 0.f;
    float phaseReactive = 0.f;
    float phaseSmooth = 0.f;

    // confidence values
    float confidence = 0.f;

private:
    void clear();
    float envelope(float t);
    
    void preCalcValues();

    int calcBestP(ldSpectrogram* spectrogram, int bestt);
    void calcOffsetPower(ldSpectrogram* spectrogram);

    // constructor args
    float m_targetTempoBpm = 120.f;
    float m_fadeHalfLife = 3.f;
    float m_trebleBias = 0.5f;
    bool m_alg = false;
    
    // helper values
    float m_m = 0.f;
    float m_n = 0.f;

    // corellograms
    static const int histLen = ldSpectrogram::histLen; // 720
    std::vector<float> offsetPower;
    std::vector<float> offsetPower2;
};

#endif /*_LDTEMPOAC_H*/
