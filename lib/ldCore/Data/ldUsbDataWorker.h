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

#ifndef LDUSBDATAWORKER_H
#define LDUSBDATAWORKER_H

#include <QtCore/QThread>

#include "ldAbstractDataWorker.h"

class ldBufferManager;
class ldHardwareManager;
class ldSimulatorEngine;
class ldThreadedDataWorker;
class ldUsbHardwareManager;

/** USB data worker */
class LDCORESHARED_EXPORT ldUsbDataWorker : public ldAbstractDataWorker
{
	Q_OBJECT
public:
    /** Constructor/destructor */
    explicit ldUsbDataWorker(ldBufferManager *bufferManager,
                              ldHardwareManager *hardwareManager,
                              ldSimulatorEngine *simulatorEngine,
                              QObject *parent = 0);
    ~ldUsbDataWorker();

    /** ldAbstractDataWorker implementations */
    virtual bool isActiveTransfer() const override;
    virtual bool hasActiveDevices() const override;

    /** USB device manager */
    ldUsbHardwareManager *deviceManager() const;

public slots:
    /** ldAbstractDataWorker implementations */
    virtual void setActive(bool active) override;
    virtual void setActiveTransfer(bool active) override;

    /** Flag to use this worker to fill sumulator buffer */
    void setSimulatorEnabled(bool enabled);

private:
    bool m_isActive = false;

    QScopedPointer<ldThreadedDataWorker> m_thread_worker;
    QThread m_worker_thread;

    ldBufferManager* m_bufferManager;
    ldHardwareManager* m_hardwareManager;
    ldSimulatorEngine* m_simulatorEngine;

    ldUsbHardwareManager* m_usbHardwareManager;
};

#endif /* LDUSBDATAWORKER_H */
