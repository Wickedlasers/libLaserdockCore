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
    m_timer.setTimerType(Qt::PreciseTimer);
    connect(&m_timer, &QTimer::timeout, this, &ldSoundStubDevice::timerSlot);
}


ldSoundStubDevice::~ldSoundStubDevice()
{
    stop();
}

void ldSoundStubDevice::start()
{
    QMutexLocker lock(&mutex);
    startStub();
}

void ldSoundStubDevice::stop()
{
//    QMutexLocker lock(&mutex);
    stopStub();
}

void ldSoundStubDevice::timerSlot()
{
    // send fake audio data (silence) to progress timer
    const int fakedatalen = 44100 / STUBFPS * 2 * 2;  // stereo int
    char fakedata[fakedatalen];
    memset(fakedata, 0, fakedatalen);
    emit soundUpdated(fakedata, fakedatalen);
}


void ldSoundStubDevice::startStub()
{
    m_timer.start(1000 / STUBFPS);
}

void ldSoundStubDevice::stopStub()
{
    m_timer.stop();
}

