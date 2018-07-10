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

// ldAppakGate.cpp

#include "ldAppakGate.h"
#include <QtCore/QDebug>
#include "ldCore/Visualizations/util/MathsHelper/ldMathStat.h"

//
ldAppakGate::ldAppakGate()
{
    memset(bufferData, 0.0, buffersize * sizeof(float));
    isStarted=false;
    startCounter=0;
    waitingTimerCounter=0;
    isSilent=true;
    previousIsSilent=false;
    maxAverage=0;
    minAverage=10000000000;
}

ldAppakGate::~ldAppakGate() { }

// basicMono
void ldAppakGate::basicMono(float mono)
{
    //
    float waveform=mono;
    
    bufferData[last_i]=waveform;
    for (int i = 1; i < buffersize; i++) {
        bufferData[i-1]=bufferData[i];
    }
    
    //
    if (!isStarted) {
        startCounter++;
        if (startCounter > buffersize) isStarted=true;
        return;
    }
        
    if (previousIsSilent!=isSilent) {
        previousIsSilent=isSilent;
    }
    
    average = ldMathStat::getFloatAverage(bufferData, buffersize);
    float max = ldMathStat::getMaxFloatValue(bufferData, buffersize);
    float min = ldMathStat::getMinFloatValue(bufferData, buffersize);
    maxAmpDiff = max -  min;
    
    debug = 0;
    if (maxAmpDiff>0) debug = fabsf(average-maxAmpDiff)/maxAmpDiff;
    
    /* isSilent3
    float ceil=0;
    if (average>maxAverage) maxAverage=average;
    if (average<minAverage && average > 0.0) minAverage=average;
    
    //
    if (10.0*minAverage > maxAverage) {
        minAverage=maxAverage/30.0;
    }
    if (100.0*minAverage < maxAverage) {
        minAverage=maxAverage/100.0;
    }
    
    ceil=minAverage*5.0;
    if (average < ceil) {
        isSilent=true;
        qDebug() << "ldAppakGate isSilent=true; ";
    } else {
        isSilent=false;
    }*/
    
    float ceil = 0.15f;
    if (average<ceil && maxAmpDiff<ceil) {
        isSilent=true;
    } else {
        isSilent=false;
    }
    
    // 
    if (!previousIsSilent && isSilent) {
        //qDebug() << "wait ";
        if (waitingTimerCounter<waitingTimer) {
            waitingTimerCounter++;
            isSilent=false;
        } else {
            waitingTimerCounter=0;
        }
    }
    //if (isSilent) qDebug()<< "silence";
    //else qDebug()<< "music";
    //qDebug()<< "waveform * 1k: " << waveform << " minAverage:" << minAverage<< " maxAverage:" << maxAverage << " average:" << average << " ceil:" << ceil;
}



