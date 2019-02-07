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

class LDCORESHARED_EXPORT ldAppakPeaks
{
public:
	ldAppakPeaks();
	~ldAppakPeaks();
    
    void process(ldSoundData* pSoundData);
    void processBpm(float bestBpm, float delta);

    float lastBpmApproximation() const;
    float output() const;
    int bpm() const;

private:
    static const int minDiff = 3*AUDIO_OVERDRIVE_FACTOR;
    static const int maxDiff = 6*AUDIO_OVERDRIVE_FACTOR;
    static const int fps = 30*AUDIO_OVERDRIVE_FACTOR;
    static const int seconds = 1;
    static const int buffersize = seconds*fps;
    static const int last_i = buffersize-1;
    static const bool debug = false;

    void doRealTimeCompute();
    void newPeak();
    void doPeakTwo();
    void doPeakOne();
    void resetPeakStats();

    //
    float m_output = 0.f;
    float m_lastBpmApproximation = 0.f; // between two last peaks

    float peakOneDebug = 0.f;
//    float peakTwoDebug = 0.f;

    float timer = 0.0f;
    float bpmTimer = 0.f;

    int startCounter = 0;
    bool isStarted = false;
    float prevOutput = 0.f;

    bool isDuringPeakOne = false;
    bool isDuringPeakTwo = false;
    bool didPeakOneReachLower = false;
    bool isPeakOneDurationReached = false;
    
    int noPeakRunningSince = 0;
    int noPeakRunningLimitSinceReset = maxDiff;
    
    float prevValForPeakTwo = 0.f;
    float prevPrevValForPeakTwo = 0.f;
    float prevPrevPrevValForPeakTwo = 0.f;
    
    //
    float powerDataOne[buffersize];
    float powerDataTwo[buffersize];

    float bufferTmp[buffersize];
    float bufferTmpTwo[buffersize];
    
    float m_milliSecondsCounter = 0;
    float m_milliSecondsCounter2 = 0;
    int m_minCurrentMillis = 500;
    bool m_isRunningBPMCounter = false;
    int bpmCount = 0;

    int m_bpm = 0;
};

#endif // LDAPPAKPEAKS_H


