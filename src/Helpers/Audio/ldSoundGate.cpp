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

#include "ldCore/Helpers/Audio/ldSoundGate.h"

#include <QtCore/QDebug>
#include "ldCore/Helpers/Maths/ldMathStat.h"


ldSoundGate::ldSoundGate()
{
    memset(bufferData, 0.0, buffersize * sizeof(float));

    isStarted=false;
    startCounter=0;
    isSilent=true;
    sliderValue=0.00f; // assume the slider is middle, between -1.00 and 1.00
    previouSliderValue=100.00f;
    computedCeilValue=0.0f;
    maxLevel=0.0f;
    minLevel=10000000.0f;
}

ldSoundGate::~ldSoundGate()
{

}

// process
void ldSoundGate::process(ldSoundData* pSoundData)
{
    if (!pSoundData) {
        qDebug()<<"ldSoundGate no pSoundData";
        return;
    }
    //
    // sliderValue=???
    sliderValue=0.6f;

    if (sliderValue!=previouSliderValue) computeSliderValue();
    //
    float waveform=0.5*(pSoundData->GetWaveformL()+pSoundData->GetWaveformR());
    waveform=1000.0*waveform;
    if (waveform<minLevel) minLevel=waveform;
    if (!isStarted) {
        startCounter++;
        if (startCounter > buffersize) isStarted=true;
    }
    bufferData[last_i]=waveform;
    for (int i = 1; i < buffersize; i++) {
        bufferData[i-1]=bufferData[i];
    }
    float average=ldMathStat::getFloatAverage(bufferData, buffersize);
    if (average > computedCeilValue) {
        isSilent=false;
    } else {
        isSilent=true;
    }

    //qDebug()<< "waveform * 1k: " << waveform << " average:" << average << " user ceil:" << computedCeilValue << " minLevel:" << minLevel;
}

// computeSliderValue
void ldSoundGate::computeSliderValue()
{
    float computedSliderValue=0;
    float res=0; // sliderValue beetween -1/1
    res=sliderValue+1.0; // value beetween 0/2
    res*=10; // value beetween 0/20
    computedSliderValue=res;
    //qDebug()<< "SliderValue: " << computedSliderValue;

    int ceilCase=floorf(computedSliderValue);
    float tempValue=0.0f;
    //qDebug()<< "ldSoundGate ceilCase: " << ceilCase;
    switch (ceilCase) {
    case 0: tempValue=0.000000001f; break;
    case 1: tempValue=0.001f; break;
    case 2: tempValue=0.01f; break;
    case 3: tempValue=0.1f; break;
    case 4: tempValue=0.5f; break;
    case 5: tempValue=1.0f; break;
    case 6: tempValue=2.0f; break;
    case 7: tempValue=4.0f; break;
    case 8: tempValue=6.0f; break;
    case 9: tempValue=8.0f; break;
    case 10: tempValue=10.0f; break; // default value
    case 11: tempValue=12.0f; break;
    case 12: tempValue=14.0f; break;
    case 13: tempValue=18.0f; break;
    case 14: tempValue=24.0f; break;
    case 15: tempValue=48.0f; break;
    case 16: tempValue=100.0f; break;
    case 17: tempValue=1000.0f; break;
    case 18: tempValue=10000.0f; break;
    case 19: tempValue=100000.0f; break;
    case 20: tempValue=10000000.0f; break;
    default: tempValue=1.f; break;
    }
    computedCeilValue=tempValue;
    previouSliderValue=sliderValue;
}
