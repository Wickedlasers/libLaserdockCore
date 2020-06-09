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

#include "ldCore/Helpers/Audio/ldBeatDetector.h"

#include <QtCore/QDebug>

#include <ldCore/Helpers/Maths/ldMaths.h>
#include <ldCore/Helpers/Maths/ldMathStat.h>

ldBeatDetector::ldBeatDetector(QObject *parent)
    : QObject(parent)
{
}

ldBeatDetector::~ldBeatDetector()
{
}

void ldBeatDetector::process(float bpm, float output, float delta)
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
                emit beatDetected();
                //
                m_isRunningBpmCounter = false;
                m_msCounter = 0;
            }
        }
    }
    m_msCounter += delta * 1000;
}

void ldBeatDetector::setDuration(float duration)
{
    m_duration = duration;
}

void ldBeatDetector::reset()
{
    m_msCounter = 0;
    m_isRunningBpmCounter = false;
    m_minCurrentMillis = 500;
}

float ldBeatDetector::progress() const
{
    Q_ASSERT(m_minCurrentMillis != 0);

    if(m_msCounter >= m_minCurrentMillis)
        return 1;

    float progressF = static_cast<float>(m_msCounter) / m_minCurrentMillis;
    return progressF;
}

