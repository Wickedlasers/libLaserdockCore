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

// ldAppakPeaks.h
//  Created by Eric Brugère on 4/13/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#ifndef LDAPPAKPEAKS_H
#define LDAPPAKPEAKS_H

#include <ldCore/Sound/ldSoundData.h>

// namespace
using namespace std;

class LDCORESHARED_EXPORT ldAppakPeaks
{
public:
	ldAppakPeaks();
	~ldAppakPeaks();
    
    void process(ldSoundData* pSoundData);
    
    //
    float output;
    float peakOneDebug;
    float peakTwoDebug;
    
    float timer = 0.0f;
    float bpmTimer;
    float lastBmpApproximation; // between two last peaks
    
protected:
private:
    static const int minDiff = 3*AUDIO_OVERDRIVE_FACTOR;
    static const int maxDiff = 6*AUDIO_OVERDRIVE_FACTOR;
    static const int fps = 30*AUDIO_OVERDRIVE_FACTOR;
    static const int seconds = 1;
    static const int buffersize = seconds*fps;
    static const int last_i = buffersize-1;
    static const bool debug = false;
    
    int startCounter;
    bool isStarted;
    float prevOutput;

    bool isDuringPeakOne;
    bool isDuringPeakTwo;
    bool didPeakOneReachLower;
    bool isPeakOneDurationReached;
    
    int noPeakRunningSince;
    int noPeakRunningLimitSinceReset;
    
    float prevValForPeakTwo;
    float prevPrevValForPeakTwo;
    float prevPrevPrevValForPeakTwo;
    
    //
    float powerDataOne[buffersize];
    float powerDataTwo[buffersize];

    float bufferTmp[buffersize];
    float bufferTmpTwo[buffersize];
    
    void doRealTimeCompute();
    void newPeak();
    void doPeakTwo();
    void doPeakOne();
    void resetPeakStats();
};

#endif // LDAPPAKPEAKS_H


