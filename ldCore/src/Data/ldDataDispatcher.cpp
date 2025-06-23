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
#include "ldCore/Hardware/ldHardwareBatch.h"
#include "ldCore/Settings/ldSettings.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"


ldDataDispatcher::ldDataDispatcher(ldFrameBuffer *frameBuffer, ldHardwareBatch *hardwareBatch, QObject *parent)
    : ldPropertyObject(parent)
    , m_simulatorEngine(new ldSimulatorEngine())    
    , m_dataWorker(new ldHardwareDataWorker(
        frameBuffer,
        hardwareBatch,
        m_simulatorEngine.get()))
{
    qDebug() << __FUNCTION__;


    connect(m_dataWorker.get(), &ldHardwareDataWorker::isActiveTransferChanged, this, [&](bool active) {
        emit activeXferChanged(active);
    });

    connect(m_dataWorker.get(), &ldHardwareDataWorker::isActiveChanged, this, [&](bool active) {
        emit activeChanged(active);
    });


    m_dataWorker->setActive(true);

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
    m_dataWorker->setActiveTransfer(active);
}

void ldDataDispatcher::setActive(bool active)
{
    m_dataWorker->setActive(active);
}
