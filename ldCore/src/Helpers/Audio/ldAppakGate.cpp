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

#include "ldCore/Helpers/Audio/ldAppakGate.h"
#include <QtCore/QDebug>
#include "ldCore/Helpers/Maths/ldMathStat.h"

//
ldAppakGate::ldAppakGate()
{
    memset(m_bufferData, 0.0, m_buffersize * sizeof(float));
}

ldAppakGate::~ldAppakGate() { }

// basicMono
void ldAppakGate::basicMono(float mono)
{
    //
    float waveform=mono;
    
    m_bufferData[m_last_i]=waveform;
    for (int i = 1; i < m_buffersize; i++) {
        m_bufferData[i-1]=m_bufferData[i];
    }
    
    //
    if (!m_isStarted) {
        m_startCounter++;
        if (m_startCounter > m_buffersize) m_isStarted=true;
        return;
    }
        
    if (m_previousIsSilent!=m_isSilent) {
        m_previousIsSilent=m_isSilent;
    }
    
    m_average = ldMathStat::getFloatAverage(m_bufferData, m_buffersize);
    float max = ldMathStat::getMaxFloatValue(m_bufferData, m_buffersize);
    float min = ldMathStat::getMinFloatValue(m_bufferData, m_buffersize);
    m_maxAmpDiff = max -  min;
    
    m_debug = 0;
    if (m_maxAmpDiff>0) m_debug = fabsf(m_average-m_maxAmpDiff)/m_maxAmpDiff;
    
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
    if (m_average<ceil && m_maxAmpDiff<ceil) {
        m_isSilent=true;
    } else {
        m_isSilent=false;
    }
    
    // 
    if (!m_previousIsSilent && m_isSilent) {
        //qDebug() << "wait ";
        if (m_waitingTimerCounter<m_waitingTimer) {
            m_waitingTimerCounter++;
            m_isSilent=false;
        } else {
            m_waitingTimerCounter=0;
        }
    }
    //if (isSilent) qDebug()<< "silence";
    //else qDebug()<< "music";
    //qDebug()<< "waveform * 1k: " << waveform << " minAverage:" << minAverage<< " maxAverage:" << maxAverage << " average:" << average << " ceil:" << ceil;
}

bool ldAppakGate::isSilent() const
{
    return m_isSilent;
}

float ldAppakGate::average() const
{
    return m_average;
}

float ldAppakGate::maxAmpDiff() const
{
    return m_maxAmpDiff;
}

float ldAppakGate::debug() const
{
   return m_debug;
}

float ldAppakGate::maxAverage() const
{
    return m_maxAverage;
}

float ldAppakGate::minAverage() const
{
    return m_minAverage;
}



