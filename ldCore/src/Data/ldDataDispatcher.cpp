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
#ifdef LASERDOCKLIB_USB_SUPPORT
    , m_isNetwork(false)
#endif
    , m_simulatorEngine(new ldSimulatorEngine())
#ifdef LASERDOCKLIB_USB_SUPPORT
    , m_usbDataWorker(new ldHardwareDataWorker(bufferManager, hardwareManager, new ldUsbHardwareManager(filterManager, this), m_simulatorEngine.get()))
#endif
    , m_networkDataWorker(new ldHardwareDataWorker(bufferManager,
                                                   hardwareManager,
                                                   new ldNetworkHardwareManager(filterManager/*, this*/), // FIXME can't have a parent if we are moving to a worker thread.
                                                   m_simulatorEngine.get()))
#ifdef LASERDOCKLIB_USB_SUPPORT
    , m_activeDataWorker( (m_isNetwork) ? m_networkDataWorker.get() : m_usbDataWorker.get())
#else
    , m_activeDataWorker(m_networkDataWorker.get())
#endif
{
    qDebug() << __FUNCTION__;

#ifdef LASERDOCKLIB_USB_SUPPORT
    connect(this, &ldDataDispatcher::isNetworkChanged, this, &ldDataDispatcher::onIsNetworkChanged);

    connect(m_usbDataWorker.get(), &ldHardwareDataWorker::isActiveTransferChanged, this, [&]() {
        emit activeChanged(m_networkDataWorker->isActiveTransfer() || m_usbDataWorker->isActiveTransfer());
    });
#endif

    connect(m_networkDataWorker.get(), &ldAbstractDataWorker::isActiveTransferChanged, this, [&]() {
        emit activeChanged(m_networkDataWorker->isActiveTransfer()
#ifdef LASERDOCKLIB_USB_SUPPORT
                           || m_usbDataWorker->isActiveTransfer()
#endif
                           );
    });


    // auto stop when hardware connects
    connect(hardwareManager, &ldHardwareManager::deviceCountChanged, this, [&](uint count) {
        if(count == 0) {
            setActiveTransfer(false);
        }
    });

    m_activeDataWorker->setActive(true);

#ifdef LASERDOCKLIB_USB_SUPPORT
    set_isNetwork(ldSettings()->value("dataDispatcher/isNetwork", m_isNetwork).toBool());
#endif
}

ldDataDispatcher::~ldDataDispatcher()
{
//    qDebug() << "~ldDataDispatcher";
}

bool ldDataDispatcher::isActiveTransfer() const
{
    return
#ifdef LASERDOCKLIB_USB_SUPPORT
        m_usbDataWorker->isActiveTransfer() ||
#endif
            m_networkDataWorker->isActiveTransfer()
            ;
}

#ifdef LASERDOCKLIB_USB_SUPPORT
ldUsbHardwareManager *ldDataDispatcher::usbDataManager() const
{
    return static_cast<ldUsbHardwareManager*>(m_usbDataWorker->deviceManager());
}
#endif

ldSimulatorEngine *ldDataDispatcher::simulatorEngine() const
{
    return m_simulatorEngine.get();
}

void ldDataDispatcher::setActiveTransfer(bool active)
{
    m_activeDataWorker->setActiveTransfer(active);
}

#ifdef LASERDOCKLIB_USB_SUPPORT
void ldDataDispatcher::onIsNetworkChanged(bool isNetwork)
{
    qDebug() << "change isNetwork to: " << isNetwork;
    m_activeDataWorker->setActiveTransfer(false);
    m_activeDataWorker->setActive(false);
    m_activeDataWorker->setSimulatorEnabled(false);

    m_activeDataWorker = isNetwork ? m_networkDataWorker.get() : m_usbDataWorker.get();


    m_activeDataWorker->setActive(true);
    m_activeDataWorker->setSimulatorEnabled(true);

    ldSettings()->setValue("dataDispatcher/isNetwork", isNetwork);
}
#endif

