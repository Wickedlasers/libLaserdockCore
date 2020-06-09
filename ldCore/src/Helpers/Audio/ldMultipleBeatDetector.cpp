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

#include "ldCore/Helpers/Audio/ldMultipleBeatDetector.h"

#include <QtCore/QDebug>

#include <ldCore/Helpers/Audio/ldBeatDetector.h>

ldMultipleBeatDetector::ldMultipleBeatDetector(const ldBeatDetector *beatDetector, QObject *parent)
    : QObject(parent)
    , m_isActive(false)
    , m_beatDetector(beatDetector)
{
    connect(this, &ldMultipleBeatDetector::isActiveChanged, this, &ldMultipleBeatDetector::onActiveChanged);
    connect(m_beatDetector, &ldBeatDetector::beatDetected, this, &ldMultipleBeatDetector::onBeatDetected);
}

ldMultipleBeatDetector::~ldMultipleBeatDetector()
{
}

void ldMultipleBeatDetector::setBeatCount(int beatCount)
{
    m_beatCount = beatCount;
    if(m_isActive)
        reset();
}

void ldMultipleBeatDetector::reset()
{
    m_detectedBeats = 0;
}

float ldMultipleBeatDetector::progress() const
{
//    qDebug() << m_detectedBeats << m_beatCount;
    Q_ASSERT(m_beatCount != 0);

    if(m_detectedBeats >= m_beatCount)
        return 1;

    float progressF = static_cast<float>(m_detectedBeats) / m_beatCount;
    float partialProgress = m_beatDetector->progress() / m_beatCount;
    return progressF + partialProgress;
}

void ldMultipleBeatDetector::onActiveChanged(bool active)
{
    if(!active)
        reset();
}

void ldMultipleBeatDetector::onBeatDetected()
{
    if(!m_isActive)
        return;

    m_detectedBeats++;
    if(m_detectedBeats >= m_beatCount) {
        m_detectedBeats = 0;
        emit beatDetected();
    }
}

