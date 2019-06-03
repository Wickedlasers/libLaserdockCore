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

#include "ldCore/Helpers/Audio/ldBpmBeatDetector.h"

#include <QtCore/QDebug>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Maths/ldMathStat.h"

ldBpmBeatDetector::ldBpmBeatDetector(QObject *parent)
    : QObject(parent)
{
}

ldBpmBeatDetector::~ldBpmBeatDetector()
{
}

void ldBpmBeatDetector::process(float bpm, float output, float delta)
{
    if (bpm < 1) bpm = 1;

    if (!m_isRunningBpmCounter) {
        m_minCurrentMillis = static_cast<int>(m_duration * 1000.f * 60.f / bpm);
        //qDebug() << "  m_minCurrentMillis" << m_minCurrentMillis;
        m_isRunningBpmCounter = true;
        m_msCounter = 0;
    } else {
        if (m_msCounter > m_minCurrentMillis) {
            // wait a peak and time out -> emit and reset
            if (output >= 0.9f ) {
                // qDebug() << "  m_milliSecondsCounter" << m_milliSecondsCounter;
                m_beatCount++;
                emit beatDetected();
                //
                m_isRunningBpmCounter = false;
                m_msCounter = 0;
            }
        }
    }
    m_msCounter+=delta * 1000;

    // calculate bpm based on peaks
    m_msBpmCounter+=delta * 1000;
    if(m_msBpmCounter > 2000) { // each 2 sec
        m_bpm = m_beatCount * 30; // 2* 30 sec = 1 min
        m_beatCount = 0;
        m_msBpmCounter = 0;
//        m_isRunningBPMCounter = false;
    }
}

int ldBpmBeatDetector::bpm() const
{
    return m_bpm;
}

void ldBpmBeatDetector::setDuration(float duration)
{
    m_duration = duration;
}

void ldBpmBeatDetector::reset()
{
    m_msCounter = 0;
    m_isRunningBpmCounter = 0;
    m_minCurrentMillis = 500;
    m_msBpmCounter = 0;
    m_beatCount = 0;
    m_bpm = 0;
}

