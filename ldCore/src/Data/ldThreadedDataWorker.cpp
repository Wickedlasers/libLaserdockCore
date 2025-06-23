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
#include "ldCore/Hardware/ldHardwareBatch.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"
#include "ldCore/Data/ldFrameBuffer.h"



namespace  {
//    const int MAX_SAMPLES_PER_PACKET = 1500;
    const int SAMPLES_PER_PACKET = 768;
    const int REMOTE_BUFFER_CUTOFF = 768;
    const int SLEEP_PERIOD_MS = 4;
}


ldThreadedDataWorker::ldThreadedDataWorker(ldFrameBuffer *frameBuffer,
                                           ldSimulatorEngine *simulatorEngine,
                                           ldHardwareBatch *hardwareBatch
                                         )
    : QObject()
    , m_simulatorEngine(simulatorEngine)
    , m_hardwareBatch(hardwareBatch)
    , m_frameBuffer(frameBuffer)
{
    connect(this, &ldThreadedDataWorker::startRun, this, &ldThreadedDataWorker::run);
}

ldThreadedDataWorker::~ldThreadedDataWorker()
{
}

void ldThreadedDataWorker::startProcess() {
    qDebug() << this <<__FUNCTION__;
    if (m_isRunning) return;
    m_isRunning = true;
    emit startRun();
}

void ldThreadedDataWorker::stopProcess() {
    qDebug() << this << __FUNCTION__;
    m_isRunning = false;
}


void ldThreadedDataWorker::run()
{
    qDebug() << this <<__FUNCTION__;
    // buffers
    std::vector<ldVertex> vertexVec(ldFrameBuffer::FRAMEBUFFER_CAPACITY);

    m_simulatedBufferFullCount = 0;
    m_simTimer.start();

    while (true) {
        if (!m_isRunning) break;

        // get the buffering config from hardware device manager
        // This must be done continuously, as the buffering strategy may change as different hardware devices
        // are added / removed (e.g. adding network wifi cube + network ethernet cube will change strategy)
        ldDeviceBufferConfigDelegator::DeviceBufferConfig cfg = m_hardwareBatch->getBufferConfig();

        // check remote buffer status and take appropriate action
        // (eg fill local buffer, send to device, or sleep)
//        const bool isSimulatorActive = m_simulatorEngine->hasListeners() && m_isSimulatorEnabled;
        const bool isSimulatorActive = m_isSimulatorEnabled;
        int remoteBuffer = -1;


        if (m_isActive)
            remoteBuffer = m_hardwareBatch->getSmallestBufferCount();


        bool isRemoteBufferAvailable = (remoteBuffer != -1);

        // simulate point count for simulator mode
        bool isSimulatorOnlyMode = !isRemoteBufferAvailable && isSimulatorActive;

        // calculate how many points would have gone out since the last update based on fixed DAC rate
        qint64 curms = m_simTimer.elapsed();
        qint64 elapsed = curms - m_lastMs;
        m_lastMs = curms;
        int simulatedDeviceSamplesPerSec = m_hardwareBatch->getCommonDACRate();
        int points = static_cast<int> ((simulatedDeviceSamplesPerSec/1000) * elapsed);

        // timer for simulated points
        if (isSimulatorOnlyMode) {
            if (elapsed > 500) elapsed = 500;
            if (elapsed > 0) {

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

        bool hasActiveUSBDevices = m_hardwareBatch->hasActiveUSBDevices();

        // if remote buffer is not available and there is no active simulator we sleep and wait for device
        if (!isRemoteBufferAvailable && !isSimulatorActive) {
            if (cfg.wait_connect_sleep_ms>0) std::this_thread::sleep_for(std::chrono::milliseconds(cfg.wait_connect_sleep_ms));
        } else {
            uint localBuffer = m_frameBuffer->getAvailable();
            uint samples_to_send = localBuffer;

            if (hasActiveUSBDevices) {

                // predict how much to send based on points calulation at the current dac rate
                if (remoteBuffer!=-1) {
                    if (remoteBuffer>REMOTE_BUFFER_CUTOFF) {
                        // once remote buffer is at our cutoff threshold, only send points at the expected DAC rate
                            samples_to_send = 0;
                    } else {
                        // we are below our remote buffer cutoff point so fill with enough samples
                        // to return to the cutoff point, if samples are available
                        if (samples_to_send > SAMPLES_PER_PACKET*2)
                             samples_to_send /= 3;
                         if (samples_to_send > SAMPLES_PER_PACKET)
                             samples_to_send /= 2;
                         if (samples_to_send > SAMPLES_PER_PACKET)
                             samples_to_send = SAMPLES_PER_PACKET;
                    }
                }
            }
            else {
                // predict how much to send based on points calulation at the current dac rate
                if (remoteBuffer!=-1) {
                    if (remoteBuffer>=cfg.remote_buffer_cutoff)  {
                        // once remote buffer is at our cutoff threshold, only send points at the expected DAC rate
                        if (samples_to_send>static_cast<uint>(points))
                            samples_to_send = points;
                    } else {
                         uint fill =(cfg.remote_buffer_cutoff-remoteBuffer);
                        samples_to_send = std::min(samples_to_send,fill);
                    }
                }

            }



            // send
            if (samples_to_send > 0) {

                ldVertex *simulatorBuffer = isSimulatorActive ? vertexVec.data() : nullptr;
                uint exhaustedIndex = m_frameBuffer->getExhuastedIndex();
                uint actualSamplesToSend = m_frameBuffer->get(simulatorBuffer, samples_to_send);

                if (actualSamplesToSend > 0) {
                    // send
                    if (m_isActive) {
                        //qDebug() << "s:" << actualSamplesToSend;
                        m_hardwareBatch->sendData(exhaustedIndex, actualSamplesToSend);
                    }
                    bool isLastPortion = (localBuffer - actualSamplesToSend == 0);
                    if (simulatorBuffer) m_simulatorEngine->pushVertexData(simulatorBuffer, actualSamplesToSend, isLastPortion);
                    if (isSimulatorOnlyMode) {
                        //qDebug() << "3434asdf" << simulatedBufferFullCount << ", " << actual;
                        m_simulatedBufferFullCount += actualSamplesToSend;
                    }
                    localBuffer -= actualSamplesToSend;
                }
            } // end if samples to send>0
            else {
                if ( hasActiveUSBDevices ) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_PERIOD_MS));
                }
            }

            if ( !hasActiveUSBDevices ) {
                std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_PERIOD_MS));
            }

            // refill if needed
            if(localBuffer == 0) {
                m_frameBuffer->reset();
                m_simulatorEngine->frame_complete();
            }

            //std::this_thread::sleep_for(std::chrono::milliseconds(4)); // fixed period to sleep until we wake to send more data
        }
    }
    // end of loop
//    qDebug() << "ldThreadedDataWorker: End of loop";
    qDebug() << this <<__FUNCTION__ << "FINISH";

    emit finished();
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
