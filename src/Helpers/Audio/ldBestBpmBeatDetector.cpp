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
// ldBestBpmBeatDetector.h
//  Created by Eric Brugère on 4/13/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Audio/ldBestBpmBeatDetector.h"

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Maths/ldMathStat.h"
#include <QtCore/QDebug>

ldBestBpmBeatDetector::ldBestBpmBeatDetector(QObject *parent)
    : QObject(parent)
{
}

ldBestBpmBeatDetector::~ldBestBpmBeatDetector()
{
}

void ldBestBpmBeatDetector::processBpm(float bestBpm, float output, float delta)
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
            if (output >= 0.9f ) {
                // qDebug() << "  m_milliSecondsCounter" << m_milliSecondsCounter;
                bpmCount++;
                emit beatDetected();
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

int ldBestBpmBeatDetector::bpm() const
{
    return m_bpm;
}

