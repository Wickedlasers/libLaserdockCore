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
#include "ldAppakSpectrum.h"
#include <QtCore/QDebug>
#include "ldCore/Visualizations/util/MathsHelper/ldMathStat.h"

//static const int historysize = 3; // keep low
static const int binSize = SAMPLE_SIZE/2;
static float binCounter[binSize];

// ldAppakSpectrum
ldAppakSpectrum::ldAppakSpectrum()
{
    frameDropper = 0;
    for (int i=0; i<SAMPLE_SIZE/2; i++) {
        frequencies.push_back(0.0);
        binCounter[i] = 0;
    }
    resSpectrum.binHzOne = 0;
    resSpectrum.binHzTwo = 0;
    resSpectrum.binHzThree = 0;
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
    frameDropper++;
    if (frameDropper > 6) frameDropper = 0;

    // all bins frequencies
    pSoundData->GetFullFFT(frequencies, true, false); // bool normalize, bool weight


    // do stats
    doSprectrumStats();

}

// doSprectrumStats
void ldAppakSpectrum::doSprectrumStats()
{
    //
    float sums[binSize];
    for (int i=0; i<binSize; i++) {
        sums[i] = frequencies[i];
        // do some clean in binCounter
        binCounter[i] *= 0.987f;
    }
    //
    int maxBin = ldMathStat::getKeyForMaxFloatValue(sums, binSize);
    binCounter[maxBin] += 2.4f;

    int maxBinCounter = ldMathStat::getKeyForMaxFloatValue(binCounter, binSize);
    //if (frameDropper==0) qDebug() << "freq:" << ((maxBinCounter+1)*30.0) << "Hz";

    resSpectrum.binHzOne = maxBinCounter;
}


// getMaxForHzInterval
int ldAppakSpectrum::getMaxBinForHzInterval(float startHz, float endHz)
{

    //
    int s = startHz / 30;
    int e = endHz / 30;
    //
    s = qMax<int>(s, 0);
    s = qMin<int>(s, 734);
    //
    e = qMax<int>(e, s + 1);
    e = qMax<int>(e, 0);
    e = qMin<int>(e, 734);

    // check max for this in binCounter
    int maxInt=s;
    float maxValue=binCounter[s];
    for (int i=s+1; i < e; i++) {
        if (binCounter[i]>maxValue) {
            maxValue=binCounter[i];
            maxInt=i;
        }
    }
    //if (frameDropper==0) qDebug() << "freq:" << ((maxInt+1)*30.0) << "Hz";

    return maxInt;
}








