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
#include "ldCore/Hardware/ldAbstractHardwareManager.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"
#include "ldCore/Data/ldBufferManager.h"
#include "ldCore/Data/ldFrameBuffer.h"



namespace  {
    const int MAX_SAMPLES_PER_PACKET = 1500;
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
    std::vector<ldVertex> vertexVec(MAX_SAMPLES_PER_PACKET);

    m_simulatedBufferFullCount = 0;
    m_simTimer.start();

    while (true) {
        if (!m_isRunning) break;

        // get the buffering config from hardware device manager
        // This must be done continuously, as the buffering strategy may change as different hardware devices
        // are added / removed (e.g. adding network wifi cube + network ethernet cube will change strategy)
        ldAbstractHardwareManager::DeviceBufferConfig cfg = m_hardwareDeviceManager->getBufferConfig();

        // check remote buffer status and take appropriate action
        // (eg fill local buffer, send to device, or sleep)
        const bool isSimulatorActive = m_simulatorEngine->hasListeners() && m_isSimulatorEnabled;
        int remoteBuffer = m_isActive ? m_hardwareDeviceManager->getBufferFullCount() : -1;
        bool isRemoteBufferAvailable = (remoteBuffer != -1);

        // simulate point count for simulator mode
        bool isSimulatorOnlyMode = !isRemoteBufferAvailable && isSimulatorActive;

        qint64 curms = m_simTimer.elapsed();
        qint64 elapsed = curms - m_lastMs;
        m_lastMs = curms;
        //qDebug() << "t=" << elapsed << "ms";

        // timer for simulated points
        if (isSimulatorOnlyMode) {
            //qint64 curms = m_simTimer.elapsed();
            //qint64 elapsed = curms - m_lastMs;
            //m_lastMs = curms;
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

        bool isRemoteBufferTooBig = (remoteBuffer > cfg.remote_buffer_cutoff);


        // if remote buffer is not available and there is no active simulator we sleep and wait for device
        if (!isRemoteBufferAvailable && !isSimulatorActive) {
            if (cfg.wait_connect_sleep_ms>0) std::this_thread::sleep_for(std::chrono::milliseconds(cfg.wait_connect_sleep_ms));
        // if remote buffer big than just sleep
        } else if (isRemoteBufferTooBig) {
            if (m_isActive) m_hardwareDeviceManager->sendData(0,0); // send null sample to trigger remote buffer size request
            if (cfg.wait_buffer_sleep_ms>0) std::this_thread::sleep_for(std::chrono::milliseconds(cfg.wait_buffer_sleep_ms));
        // everything is ok, process buffer flow
        } else {
            // count local
            uint localBuffer = m_frameBuffer->getAvailable();

            // send
            if (localBuffer > 0) {

                // try to divide into evenly sized pieces if we're over the size limit
                uint samples_to_send = localBuffer;

                if (cfg.max_samples_per_udp_xfer==0 || isSimulatorActive){
                    if (samples_to_send > cfg.samples_per_packet*2)
                        samples_to_send /= 3;
                    if (samples_to_send > cfg.samples_per_packet)
                        samples_to_send /= 2;
                    if (samples_to_send > cfg.samples_per_packet)
                        samples_to_send = cfg.samples_per_packet;
                }

                ldVertex *simulatorBuffer = isSimulatorActive ? vertexVec.data() : nullptr;
                uint exhaustedIndex = m_frameBuffer->getExhuastedIndex();
                uint actualSamplesToSend = m_frameBuffer->get(simulatorBuffer, samples_to_send);

                if (actualSamplesToSend > 0) {
                    // send
                    if (m_isActive) m_hardwareDeviceManager->sendData(exhaustedIndex, actualSamplesToSend);
                    if (simulatorBuffer) m_simulatorEngine->pushVertexData(simulatorBuffer, actualSamplesToSend);
                    if (isSimulatorOnlyMode) {
                        //qDebug() << "3434asdf" << simulatedBufferFullCount << ", " << actual;
                        m_simulatedBufferFullCount += actualSamplesToSend;
                    }
                    localBuffer -= actualSamplesToSend;

                    if (cfg.sleep_after_packet_send_ms>0) std::this_thread::sleep_for(std::chrono::milliseconds(cfg.sleep_after_packet_send_ms));

                }
//                qDebug() << "remote buffer size is " << remoteBuffer << "\t" << localBuffer << "\t" << actual;
            }

            // refill if needed
            if(localBuffer == 0) {
                m_frameBuffer->reset();
            }
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

void ldThreadedDataWorker::setHardwareDeviceManager(ldAbstractHardwareManager *hardwareDeviceManager)
{
    m_hardwareDeviceManager = hardwareDeviceManager;
}
