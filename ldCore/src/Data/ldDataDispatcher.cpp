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

#include <ldCore/Data/ldDataDispatcher.h>

#include <QtCore/QDebug>
#include <QtCore/QMetaObject>

#include "ldCore/ldCore.h"
#include "ldCore/Data/ldAbstractDataWorker.h"
#include "ldCore/Data/ldBufferManager.h"
#include "ldCore/Data/ldHardwareDataWorker.h"
#include "ldCore/Hardware/ldHardware.h"
#include "ldCore/Hardware/ldHardwareManager.h"
#include "ldCore/Hardware/ldNetworkHardwareManager.h"
#include "ldCore/Settings/ldSettings.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"

#ifdef LASERDOCKLIB_USB_SUPPORT
#include "ldCore/Hardware/ldUSBHardwareManager.h"
#endif

ldDataDispatcher::ldDataDispatcher(ldBufferManager *bufferManager, ldHardwareManager *hardwareManager, ldFilterManager* filterManager, QObject *parent)
    : ldPropertyObject(parent)
    , m_simulatorEngine(new ldSimulatorEngine())    
    , m_dataWorker(new ldHardwareDataWorker(
        bufferManager,
        hardwareManager,
        std::vector<ldAbstractHardwareManager*>{
            static_cast<ldAbstractHardwareManager*>(new ldNetworkHardwareManager(filterManager)),
            #ifdef LASERDOCKLIB_USB_SUPPORT
                static_cast<ldAbstractHardwareManager*>(new ldUsbHardwareManager(filterManager, this))
            #endif
        },
        m_simulatorEngine.get()))
    , m_activeDataWorker(m_dataWorker.get())
{
    qDebug() << __FUNCTION__;


    connect(m_dataWorker.get(), &ldHardwareDataWorker::isActiveTransferChanged, this, [&]() {
        emit activeChanged(m_dataWorker->isActiveTransfer());
    });

    // auto stop when hardware connects
    connect(hardwareManager, &ldHardwareManager::deviceCountChanged, this, [&](uint count) {
        if(count == 0) {
            setActiveTransfer(false);
        }
    });

    m_activeDataWorker->setActive(true);

}

ldDataDispatcher::~ldDataDispatcher()
{
//    qDebug() << "~ldDataDispatcher";
}

bool ldDataDispatcher::isActiveTransfer() const
{
    return m_dataWorker->isActiveTransfer();
}

ldSimulatorEngine *ldDataDispatcher::simulatorEngine() const
{
    return m_simulatorEngine.get();
}

void ldDataDispatcher::setActiveTransfer(bool active)
{
    m_activeDataWorker->setActiveTransfer(active);
}

