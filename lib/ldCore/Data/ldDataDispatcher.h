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

#ifndef LDDATADISPATCHER_H
#define LDDATADISPATCHER_H

#include <memory>

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

class ldBufferManager;
class ldAbstractDataWorker;
class ldHardwareManager;
class ldSimulatorEngine;
class ldUsbDataWorker;

class LDCORESHARED_EXPORT ldDataDispatcher : public QObject
{
    Q_OBJECT
    
public:
    explicit ldDataDispatcher(ldBufferManager *bufferManager,
                              ldHardwareManager *hardwareManager,
                              QObject *parent = 0);
    ~ldDataDispatcher();
    
    bool isActiveTransfer() const;

    void setAdditionalDataWorker(ldAbstractDataWorker *dataWorker);

    ldUsbDataWorker* usbDataWorker() const;

public slots:
    ldSimulatorEngine* simulatorEngine() const;
    void loadSimulator();
    void unloadSimulator();

    void setActiveTransfer(bool active);

signals:
    void activeChanged(bool active);

private:
    ldHardwareManager *m_hardwareManager;

    std::unique_ptr<ldSimulatorEngine> m_simulatorEngine;

    std::unique_ptr<ldUsbDataWorker> m_usbDataWorker;
    ldAbstractDataWorker *m_additionalDataWorker = nullptr;
};

#endif // LDDATADISPATCHER_H
