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

// ;
// ldAppakPeaks.h
//  Created by Eric Brugère on 4/13/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Audio/ldAppakPeaks.h"

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Maths/ldMathStat.h"
#include <QtCore/QDebug>

// ldAppakPeaks
ldAppakPeaks::ldAppakPeaks() {
    memset(powerDataOne, 0.0, buffersize * sizeof(float));
    memset(powerDataTwo, 0.0, buffersize * sizeof(float));
    memset(bufferTmp, 0.0, buffersize * sizeof(float));
    memset(bufferTmpTwo, 0.0, buffersize * sizeof(float));
}

// ~ldAppakPeaks
ldAppakPeaks::~ldAppakPeaks() {
    // clear buffers
}

// process
void ldAppakPeaks::process(ldSoundData* pSoundData)
{
    if (!pSoundData) return;
    //qDebug() << "process";
    //
    if (!isStarted) {
        startCounter++;
        if (startCounter > buffersize) isStarted=true;
    }

    // history
    for (int i=1; i<buffersize; i++) {
        powerDataOne[i-1] = powerDataOne[i];
        powerDataTwo[i-1] = powerDataTwo[i];
    }
    
    // feed powerDataOne
    float val = 0.5 * ( pSoundData->GetWaveformL() + pSoundData->GetWaveformR() );
    powerDataOne[last_i] = val;
    
    // feed powerDataTwo
    val = 0;
    for (int i=0;i<300; i++) {
        float power=pSoundData->powerAt(i,300);
        if (40<i && i<100) val += powf(power, 10);
        else if (100<=i && i<130) val += powf(power, 6);
        else val += powf(power, 3);
    }
    powerDataTwo[last_i] = val;
    
    // doRealTimeCompute
    m_output = 0.0;
    if (isStarted) doRealTimeCompute();
    
    if (prevOutput == 1.0) {
        m_output = 0.99f;
    }

    // update bpm Approx
    if (m_output >= 1.0) {
        if (bpmTimer > 0) {
            m_lastBpmApproximation = 60.0f/bpmTimer;
            //qDebug() << "inv " << inv*60.0;
        }
        bpmTimer = 0;
    } else {
        bpmTimer += 1.0f/fps;
    }

    //
    prevOutput=m_output;
}

void ldAppakPeaks::processBpm(float bestBpm, float delta)
{
    if (bestBpm < 1) bestBpm = 1;


    if (!m_isRunningBPMCounter) {
        m_minCurrentMillis = static_cast<int>(500.f * 60.f / bestBpm);
        //qDebug() << "  m_minCurrentMillis" << m_minCurrentMillis;
        m_isRunningBPMCounter = true;
        m_milliSecondsCounter = 0;
    } else {
        if (m_milliSecondsCounter > m_minCurrentMillis) {
            // wait a peak and time out -> emit and reset
            if (m_output >= 0.9f ) {
                // qDebug() << "  m_milliSecondsCounter" << m_milliSecondsCounter;
                bpmCount++;
                //
                m_isRunningBPMCounter = false;
                m_milliSecondsCounter = 0;
            }
        }
    }

    //
    m_milliSecondsCounter+=delta * 1000;
    m_milliSecondsCounter2+=delta * 1000;

    if(m_milliSecondsCounter2 > 2000) { // each 2 sec
        m_bpm = bpmCount * 30; // 2* 30 sec = 1 min

        bpmCount = 0;
        m_milliSecondsCounter2 = 0;
        m_isRunningBPMCounter = false;
    }
}

float ldAppakPeaks::lastBpmApproximation() const
{
    return m_lastBpmApproximation;
}

float ldAppakPeaks::output() const
{
    return m_output;
}

int ldAppakPeaks::bpm() const
{
    return m_bpm;
}

// doRealTimeCompute
void ldAppakPeaks::doRealTimeCompute() {
    
    doPeakTwo();
    doPeakOne();
    
    //
    if (timer > 0.0) {
        timer = timer - AUDIO_OVERDRIVE_FACTOR;
    
        if (timer < minDiff && didPeakOneReachLower && !isPeakOneDurationReached) {
            isPeakOneDurationReached = true;
            timer = minDiff;
        }
        
        // peak run limit reached
        if (timer <= 0.001) {
            resetPeakStats();
        }
    }

    //
    if (noPeakRunningSince > noPeakRunningLimitSinceReset || timer < 0.0) {
        resetPeakStats();
    } else if (!isDuringPeakOne && !isDuringPeakTwo) noPeakRunningSince++;
    
    
}

// resetPeakStats
void ldAppakPeaks::resetPeakStats()
{
    noPeakRunningSince=0;
    isDuringPeakOne=false;
    isDuringPeakTwo=false;
    didPeakOneReachLower=false;
    timer = 0.0;
}

// newPeak
void ldAppakPeaks::newPeak()
{
    noPeakRunningSince=0;
    timer = 4.0*minDiff;
    m_output = 1.0;
}

// doPeakOne
void ldAppakPeaks::doPeakOne()
{
    // PEAK ONE : try with exp powered
    // m->audioBasic->mono*ldMaths::normExp(m->audioBasic->mono, 4)*ldMaths::normExp(m->audioBasic->mono, 8);
    ldMathStat::copyArray(powerDataOne, bufferTmp, buffersize);
    float maxValue = ldMathStat::getMaxFloatValue(bufferTmp, buffersize);
    ldMathStat::normalizeBuffer(bufferTmp, buffersize, maxValue);
    //
    float lastVal = bufferTmp[last_i];
    lastVal = lastVal*ldMaths::normExp(lastVal, 4)*ldMaths::normExp(lastVal, 8);
    float ceil = 0.06f;
    
    peakOneDebug = 0.6*lastVal;
    
    //
    if (isDuringPeakOne && lastVal < ceil) didPeakOneReachLower=true;

    // hit the ceil, surely a peak
    if (timer == 0.0 && lastVal > ceil) {
        isDuringPeakOne = true;
        newPeak();
        isPeakOneDurationReached = false;
        didPeakOneReachLower = false;
    }
}

// doPeakTwo
void ldAppakPeaks::doPeakTwo()
{
    // PEAK TWO (just like in Appakabar beat but with other params)
    ldMathStat::copyArray(powerDataTwo, bufferTmp, buffersize);
    float maxValue = ldMathStat::getMaxFloatValue(bufferTmp, buffersize);
    ldMathStat::normalizeBuffer(bufferTmp, buffersize, maxValue);
    for (int i = 1; i < buffersize; i++) {
        bufferTmpTwo[i]=bufferTmp[i]-bufferTmp[i-1];
    }
    maxValue=ldMathStat::getMaxFloatValue(bufferTmpTwo, buffersize);
    ldMathStat::normalizeBuffer(bufferTmpTwo, buffersize, maxValue);
    float averageEnergy = ldMathStat::getFloatAverage(bufferTmpTwo, buffersize);
    float deviationEnergy = ldMathStat::getFloatDeviation(bufferTmpTwo, buffersize, averageEnergy);
    //
    bool test1 = bufferTmpTwo[last_i] > (averageEnergy + deviationEnergy);
    bool test2 = (prevPrevPrevValForPeakTwo + prevPrevValForPeakTwo + prevValForPeakTwo +  bufferTmpTwo[last_i])/4 > 0.3;
    
   // peakTwoDebug = (prevPrevPrevValForPeakTwo + prevPrevValForPeakTwo + prevValForPeakTwo +  bufferTmpTwo[last_i])/4;
//    if (test1) peakTwoDebug = 1.0;
//    else if (test2) peakTwoDebug = 0.80;
//    else peakTwoDebug = 0.0;
//    
    if (timer == 0.0) {
        if (test1 || test2) {
            isDuringPeakTwo=true;
            newPeak();
        }
    }

    prevPrevPrevValForPeakTwo = prevPrevValForPeakTwo;
    prevPrevValForPeakTwo = prevValForPeakTwo;
    prevValForPeakTwo = bufferTmpTwo[last_i];
}

