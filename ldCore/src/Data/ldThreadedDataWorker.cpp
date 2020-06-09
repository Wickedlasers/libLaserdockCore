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

#include "ldThreadedDataWorker.h"

#include <thread>

#include <QtCore/QtDebug>
#include <QtCore/QDateTime>
#include <QtCore/QElapsedTimer>
#include <QtCore/QThread>
#include <QtGui/QGuiApplication>

#include "ldCore/Hardware/ldHardware.h"
#include "ldCore/Hardware/ldHardwareManager.h"
#include "ldCore/Hardware/ldUSBHardwareManager.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"

#include "ldCore/Data/ldBufferManager.h"
#include "ldCore/Data/ldFrameBuffer.h"


namespace  {
const uint SAMPLES_PER_PACKET = ldHardware::REMOTE_MAX_BUFFER;
const int REMOTE_BUFFER_CUTOFF = 512;
const int WAIT_CONNECT_SLEEP_MS = 12;
const int WAIT_BUFFER_SLEEP_MS = 6;
}

ldThreadedDataWorker::ldThreadedDataWorker(ldBufferManager *bufferManager,
                                         ldSimulatorEngine *simulatorEngine
                                         )
    : m_bufferManager(bufferManager)
    , m_simulatorEngine(simulatorEngine)
    , m_frameBuffer(new ldFrameBuffer(this))
{
    bufferManager->registerBuffer(m_frameBuffer);

    connect(this, &ldThreadedDataWorker::startRun, this, &ldThreadedDataWorker::run);
}

ldThreadedDataWorker::~ldThreadedDataWorker()
{
}

void ldThreadedDataWorker::startProcess() {
    if (m_isRunning) return;
    m_isRunning = true;
    emit startRun();
}

void ldThreadedDataWorker::stopProcess() {
    m_isRunning = false;
}

void ldThreadedDataWorker::run()
{
    // buffers
    std::vector<ldVertex> vertexVec(SAMPLES_PER_PACKET);

    m_simulatedBufferFullCount = 0;
    m_simTimer.start();

    while (true) {
        if (!m_isRunning) break;

        // check remote buffer status and take appropriate action
        // (eg fill local buffer, send to device, or sleep)
        const bool isSimulatorActive = m_simulatorEngine->hasListeners() && m_isSimulatorEnabled;
        int remoteBuffer = m_isActive ? m_usbDeviceManager->getBufferFullCount() : -1;
        bool isRemoteBufferAvailable = (remoteBuffer != -1);

        // simulate point count for simulator mode
        bool isSimulatorOnlyMode = !isRemoteBufferAvailable && isSimulatorActive;

        // timer for simulated points
        if (isSimulatorOnlyMode) {
            //qint64 elapsed = m_sim_timer.restart();
            //qint64 curms = QDateTime::currentMSecsSinceEpoch();
            qint64 curms = m_simTimer.elapsed();
            qint64 elapsed = curms - m_lastMs;
            m_lastMs = curms;
            if (elapsed > 500) elapsed = 500;
            const int simulatedDeviceSamplesPerSec = 30000;
            if (elapsed > 0) {
                int points = static_cast<int> ((simulatedDeviceSamplesPerSec/1000) * elapsed);
                m_simulatedBufferFullCount -= points;
                if (m_simulatedBufferFullCount < 0) {
                    // note - simulates buffer underrun
                    // should only happen when first starting up the mode
                    m_simulatedBufferFullCount = 0;
                }
            }
            // use sim buffer in place of real device data
            remoteBuffer = m_simulatedBufferFullCount;
            isRemoteBufferAvailable = true;
        }

        bool isRemoteBufferTooBig = (remoteBuffer > REMOTE_BUFFER_CUTOFF);

        // if remote buffer is not available and there is no active simulator we sleep and wait for device
        if (!isRemoteBufferAvailable && !isSimulatorActive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_CONNECT_SLEEP_MS));
        // if remote buffer big than just sleep
        } else if (isRemoteBufferTooBig) {
            std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_BUFFER_SLEEP_MS));
        // everything is ok, process buffer flow
        } else {
            // count local
            uint localBuffer = m_frameBuffer->getAvailable();

            // send
            if (localBuffer > 0) {

                // try to divide into evenly sized pieces if we're over the size limit
                // NOTE: probably not necessary since later it is checked for available remote buffer
                // but can be useful for smooth data sending
                uint samples_to_send = localBuffer;
                if (samples_to_send > SAMPLES_PER_PACKET*2)
                    samples_to_send /= 3;
                if (samples_to_send > SAMPLES_PER_PACKET)
                    samples_to_send /= 2;
                if (samples_to_send > SAMPLES_PER_PACKET)
                    samples_to_send = SAMPLES_PER_PACKET;

                // check for case when samples to send is more than availalbe remote buffer
                Q_ASSERT(remoteBuffer >= 0);
                uint availableRemoteBuffer = ldHardware::REMOTE_MAX_BUFFER - static_cast<uint>(remoteBuffer);
                if(samples_to_send > availableRemoteBuffer)
                    samples_to_send = availableRemoteBuffer;

                ldVertex *simulatorBuffer = isSimulatorActive ? vertexVec.data() : nullptr;
                uint exhaustedIndex = m_frameBuffer->getExhuastedIndex();
                uint actualSamplesToSend = m_frameBuffer->get(simulatorBuffer, samples_to_send);
                if (actualSamplesToSend > 0) {
                    // send
                    if (m_isActive) m_usbDeviceManager->sendData(exhaustedIndex, actualSamplesToSend);
                    if (simulatorBuffer) m_simulatorEngine->pushVertexData(simulatorBuffer, actualSamplesToSend);
                    if (isSimulatorOnlyMode) m_simulatedBufferFullCount += actualSamplesToSend;

                    localBuffer -= actualSamplesToSend;
                }
//                qDebug() << "remote buffer size is " << remoteBuffer << "\t" << localBuffer << "\t" << actual;
            }

            // refill if needed
            if(localBuffer == 0)
                m_frameBuffer->reset();
        }
    }
    // end of loop
//    qDebug() << "ldThreadedDataWorker: End of loop";
}

bool ldThreadedDataWorker::isActiveTransfer() const
{
    return m_isActive;
}

void ldThreadedDataWorker::setActiveTransfer(bool active)
{
    if(m_isActive == active) {
        return;
    }

    m_isActive = active;

    emit activeChanged(active);
}

void ldThreadedDataWorker::setSimulatorEnabled(bool enabled)
{
    m_isSimulatorEnabled = enabled;
}

void ldThreadedDataWorker::setUsbDeviceManager(ldUsbHardwareManager *usbDeviceManager)
{
    m_usbDeviceManager = usbDeviceManager;
}
