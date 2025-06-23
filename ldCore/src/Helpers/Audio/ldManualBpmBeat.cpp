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

#include "ldCore/Helpers/Audio/ldManualBpmBeat.h"

#include <QtCore/QDebug>

#include <ldCore/Helpers/Audio/ldManualBpm.h>
#include <ldCore/Helpers/Maths/ldGlobals.h>

ldManualBpmBeat::ldManualBpmBeat(ldManualBpm *manualBpm, QObject *parent)
    : QObject(parent)
    , m_manualBpm(manualBpm)
{
    connect(&m_updateTimer, &QTimer::timeout, this, &ldManualBpmBeat::updateCycleValue);
    m_updateTimer.setInterval(1000 / 50); //  50 times per second
}


void ldManualBpmBeat::restart()
{
    m_elapsedTimer.restart();
    m_updateTimer.start();
}

void ldManualBpmBeat::start()
{
    m_elapsedTimer.start();
    m_updateTimer.start();
}

void ldManualBpmBeat::stop()
{
    m_updateTimer.stop();
    m_elapsedTimer.invalidate();
}

void ldManualBpmBeat::updateCycleValue()
{
    float bpm = m_manualBpm->bpm();

    if(!m_elapsedTimer.isValid() || cmpf(bpm, 0))
        return;

    float elapsed_secs = m_diff + (m_elapsedTimer.elapsed() / 1000.f);
    float bps = (60.f / bpm);

    if( elapsed_secs>= bps ) {
        emit beatDetected();
        m_diff = elapsed_secs - bps;
        m_elapsedTimer.restart();
    }
}

