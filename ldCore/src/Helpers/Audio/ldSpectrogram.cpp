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

#include "ldCore/Helpers/Audio/ldSpectrogram.h"

#include <QtCore/QtDebug>

#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

ldSpectrogram::ldSpectrogram() {
    m_cachedS.resize(histLen, 0);
}

void ldSpectrogram::addFrame(ldSpectrumFrame &f) {
    head = (head + 1) % histLen;
    hist[head] = f;
}

ldSpectrumFrame ldSpectrogram::getNoisedFrame(float histSec, float rootScale, float /*rootPower*/, bool alt1, bool alt2) {

    const int slsLen = ldSpectrumFrame::slsLen;
    int thistLen = (histSec)*30*AUDIO_OVERDRIVE_FACTOR;
    thistLen = MIN(thistLen, histLen);

    ldSpectrumFrame rootFrame = currentFrame();
    for (int i = 0; i < slsLen; i++) {
        float cutoff = rootFrame.sls[i];
        float av = cutoff;
        if (alt1) {
            float sum = 0; float total = 0;
            for (int j = 0; j < thistLen; j++) {
                int index = (head - j + histLen) % histLen;;
                float f = (float) (thistLen - j) / thistLen;
                sum += f*hist[index].sls[i]; total += f;
            }
            if (total > 0) sum /= total;
            cutoff = av = sum;
        }
        for (int j = 0; j < thistLen; j++) {
            int index = (head - j + histLen) % histLen;;
            float f = (float) (thistLen - j) / thistLen;
            float cf = 1-f;
            float t = f*hist[index].sls[i] + cf*av;
            if (t < cutoff) cutoff = t;
        }
        cutoff *= rootScale;
        if (cutoff > 0) {
            float &t = rootFrame.sls[i];
            float n = t / cutoff;

            float x = (n/2); clampfp(x, 0, 1);
            t -= x*cutoff;

            /*
            float ta = t - cutoff;
            if (ta <  0) ta = 0;

            float tb = t;
            if (tb < cutoff) {
                tb /= cutoff;
                tb = powf(t, rootPower);
                tb *= cutoff;
            }

            float x = (n/3); clampfp(x, 0, 1);
            float xc = 1-x;

            t = x*ta + xc*tb;*/

            //if (t > cutoff) {
                //float x = (n/3); clampfp(x, 0, 1);
                //t -= x*cutoff;
            //}
            /*if (t < cutoff) {
                t /= cutoff;
                t = powf(t, rootPower);
                t *= cutoff;
            }*/
        }
    }

    if (alt2) {
        float tlb = 0, tla = 0;
        for (int i = 0; i < slsLen; i++) {
            tlb += currentFrame().sls[i];
            tla += rootFrame.sls[i];
        }
        if (tla > 0) {
            float f = tlb / tla;
            if (f > 2) f = 2;
            for (int i = 0; i < slsLen; i++) rootFrame.sls[i] *= f;
        }
    }

    return rootFrame;

}

void ldSpectrogram::calculateS()
{
    const ldSpectrumFrame &h1 = currentFrame();

    int total = ldSpectrumFrame::slsLen * 3;

    // 720 * 8 = 5760 iterations per frame...
    for(int t = 1; t < histLen; t++) {
        float sum = 0;

        const ldSpectrumFrame &h2 = frame(head - t);

        // 8 iterations
        for (int i = 0; i < ldSpectrumFrame::slsLen; i++) {
            //float f1 = slsHist[i1][i];
            //float f2 = slsHist[i2][i];
            float f1 = h1.sls[i];
            float f2 = h2.sls[i];

            float d1 = fabsf(f1 - f2) / (f1 + f2);

            float d2 = (f1 - f2) / (f1 + f2);
            if (d2 < 0) d2 = 0;

            float d3 = (f1) / (f1 + f2);
            d3 = (0.5-d3)*(0.5-d3);

            float ld1 = (1/(d1+1));
            float ld2 = (1/(d2+1));
            float ld3 = (1/(d3+1));

            clampfp(ld1, 0, 1);
            clampfp(ld2, 0, 1);
            clampfp(ld3, 0, 1);

            sum += ld1;
            sum += ld2;
            sum += ld3;
        }

        m_cachedS[t] = sum/total;
    }
}


float ldSpectrogram::getS(int t) const
{
    return m_cachedS[t];
}

const ldSpectrumFrame &ldSpectrogram::currentFrame() const
{
    return hist[head];
}

const ldSpectrumFrame &ldSpectrogram::frame(int index) const
{
    if(index < 0 || index >= histLen) {
        index = (index + histLen) % histLen;
    }

    return hist[index];
}
