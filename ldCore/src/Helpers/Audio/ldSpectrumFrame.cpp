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

#include "ldCore/Helpers/Audio/ldSpectrumFrame.h"

#include <QtCore/QtDebug>

#include <ldCore/Sound/ldSoundData.h>

ldSpectrumFrame::ldSpectrumFrame() {
    sls.resize(slsLen);
}

void ldSpectrumFrame::update(ldSoundData* psd) {
    // calculate sls spectrum from raw spectrum
    float pre = 1; // preamp
    int fptr = 1; // index of bin
    int w = 2; // width of bin conosolidaiton
    for (int i = 0; i < slsLen; i++) {
        float sum = 0;
        float total = 0;
        for (int j = 0; j < w; j++) {
            float z = psd->GetFFTValueForFrequency(30.0*(fptr), 0);
            float m = fptr;//sqrt(fptr); // multiplier
            sum += z*m;
            total += m;
            fptr++; // advance to next bin
            if (fptr >= 1470) qWarning() << "error in sls";
        }
        sls[i] = w*pre*sum/total;
        w *= 2; // next one has twice as many bins
    }
}

float ldSpectrumFrame::getEnergy() {
    return 0.5;
}

float ldSpectrumFrame::getEnergyScaled(float f) const {
    float sum = 0, total = 0;
    for (int i = 0; i < slsLen; i++) {
        float s = (i + 0.5) / slsLen;
        s = 0.5+(s-0.5)*2*(f-0.5)*2;
        if (s < 0) s = 0;
        if (s > 1) s = 1;
        sum += sls[i]*s;
        total += s;
    }
    return sum / total;
}

float ldSpectrumFrame::compare(const ldSpectrumFrame* p1, const ldSpectrumFrame* p2) {
    float f = 0.5;
    float sum = 0, total = 0;
    for (int i = 0; i < slsLen; i++) {
        float s = (i + 0.5) / slsLen;
        s = 0.5+(s-0.5)*2*(f-0.5)*2;
        if (s < 0) s = 0;
        if (s > 1) s = 1;
        float sss = fmin(p1->sls[i], p2->sls[i]);
        sum += sss*s;
        total += s;
    }
    return sum / total;
}
