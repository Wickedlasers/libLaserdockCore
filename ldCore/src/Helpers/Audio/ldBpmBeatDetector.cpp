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

#include "ldCore/Helpers/Audio/ldBpmBeatDetector.h"

#include <QtCore/QDebug>

#include <ldCore/Helpers/Maths/ldMaths.h>
#include <ldCore/Helpers/Maths/ldMathStat.h>

#include <ldCore/Helpers/Audio/ldBeatDetector.h>

ldBpmBeatDetector::ldBpmBeatDetector(ldBeatDetector *beatDetector, QObject *parent)
    : QObject(parent)
    , m_beatDetector(beatDetector)
{
    connect(beatDetector, &ldBeatDetector::beatDetected, this, [&](){
        m_beatCount++;
    });
}

ldBpmBeatDetector::~ldBpmBeatDetector()
{
}

void ldBpmBeatDetector::process(float delta)
{
    // calculate bpm based on peaks
    m_msBpmCounter += delta * 1000;
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

void ldBpmBeatDetector::reset()
{
    m_msBpmCounter = 0;
    m_beatCount = 0;
    m_bpm = 0;
}

