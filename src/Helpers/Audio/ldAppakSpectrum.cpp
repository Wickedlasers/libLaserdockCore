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
//  ldAppakSpectrum.cpp
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//
#include "ldCore/Helpers/Audio/ldAppakSpectrum.h"
#include <QtCore/QDebug>
#include "ldCore/Helpers/Maths/ldMathStat.h"

namespace {
    static const int binSize = SAMPLE_SIZE/2;
}

// ldAppakSpectrum
ldAppakSpectrum::ldAppakSpectrum()
{
    m_binCounter.resize(binSize, 0);
    frequencies.resize(binSize, 0);
}

// ~ldAppakSpectrum
ldAppakSpectrum::~ldAppakSpectrum() { }

// process
void ldAppakSpectrum::process(ldSoundData* pSoundData)
{
    if (!pSoundData) {
        qDebug()<<"ldAppakSpectrum no pSoundData";
        return;
    }
    m_frameDropper++;
    if (m_frameDropper > 6) m_frameDropper = 0;

    // all bins frequencies
    pSoundData->GetFullFFT(frequencies, true, false); // bool normalize, bool weight


    // do stats
    doSprectrumStats();

}


// getMaxForHzInterval
int ldAppakSpectrum::getMaxBinForHzInterval(float startHz, float endHz) const
{

    //
    int s = static_cast<int>(startHz / 30);
    int e = static_cast<int>(endHz / 30);
    //
    s = qMax<int>(s, 0);
    s = qMin<int>(s, 734);
    //
    e = qMax<int>(e, s + 1);
    e = qMax<int>(e, 0);
    e = qMin<int>(e, 734);

    // check max for this in binCounter
    int maxInt=s;
    float maxValue=m_binCounter[s];
    for (int i=s+1; i < e; i++) {
        if (m_binCounter[i]>maxValue) {
            maxValue=m_binCounter[i];
            maxInt=i;
        }
    }
    //if (frameDropper==0) qDebug() << "freq:" << ((maxInt+1)*30.0) << "Hz";

    return maxInt;
}






// doSprectrumStats
void ldAppakSpectrum::doSprectrumStats()
{
    //
    float sums[binSize];
    for (uint i=0; i<binSize; i++) {
        sums[i] = frequencies[i];
        // do some clean in binCounter
        m_binCounter[i] *= 0.9f;//987f;
    }
    //
    int maxBin = ldMathStat::getKeyForMaxFloatValue(sums, binSize);
    //binCounter[maxBin] += 2.4f;
    m_binCounter[maxBin] += 1.4f;

    int maxBinCounter = ldMathStat::getKeyForMaxFloatValue(m_binCounter.data(), binSize);
    //if (frameDropper==0) qDebug() << "freq:" << ((maxBinCounter+1)*30.0) << "Hz";

    resSpectrum.binHzOne = maxBinCounter;
}



