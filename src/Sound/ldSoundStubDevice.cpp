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

#include "ldSoundStubDevice.h"

ldSoundStubDevice::ldSoundStubDevice(QObject *parent) :
    QObject(parent)
{
    memset(m_fakeData, 0, FAKEDATA_LEN);

    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &ldSoundStubDevice::timerSlot);
}


ldSoundStubDevice::~ldSoundStubDevice()
{
    stop();
}

void ldSoundStubDevice::start()
{
    QMutexLocker lock(&m_mutex);
    m_timer.start(1000 / STUBFPS);
}

void ldSoundStubDevice::stop()
{
    QMutexLocker lock(&m_mutex);
    m_timer.stop();
}

void ldSoundStubDevice::timerSlot()
{
    // send fake audio data (silence) to progress timer
    emit soundUpdated(m_fakeData, FAKEDATA_LEN);
}



