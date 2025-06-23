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

#include <ldCore/Data/ldDataDispatcherManager.h>

#include <QtCore/QCoreApplication>

#include <ldCore/ldCore.h>
#include <ldCore/Data/ldDataDispatcher.h>
#include <ldCore/Data/ldFrameBuffer.h>
#include <ldCore/Hardware/ldHardwareBatch.h>
#include <ldCore/Hardware/ldHardwareManager.h>
#include <ldCore/Helpers/ldLaserController.h>
#include <ldCore/Shape/ldShape.h>
#include <ldCore/Task/ldTaskManager.h>
#include <ldCore/Task/ldTaskWorker.h>
#include <ldCore/Visualizations/ldVisualizationTask.h>


ldDataDispatcherManager::ldDataDispatcherManager(QObject *parent)
    : ldPropertyObject(parent)
    , m_count(0)
{
    qDebug() << __FUNCTION__;

    // auto stop when all hardwares diconnect
    connect(ldCore::instance()->hardwareManager(), &ldHardwareManager::deviceCountChanged, this, [&](uint count) {
        if(count == 0) {
            setActiveTransfer(false);
        }
    });
}

ldDataDispatcherInstance* ldDataDispatcherManager::createNew(const QStringList &ids)
{
    ldHardwareBatch* hwBatch = new ldHardwareBatch(ldCore::instance()->hardwareManager());
    if(!ids.empty())
        hwBatch->setIds(ids);

    ldFrameBuffer* frameBuffer = new ldFrameBuffer(hwBatch, this); // TODO delete too manually
    ldDataDispatcher* dataDispatcher = new ldDataDispatcher(frameBuffer, hwBatch);

    // create and load task
    ldTaskWorker* taskWorker = ldCore::instance()->taskManager()->createTaskWorker(frameBuffer);

    // load task
    ldVisualizationTask* task = new ldVisualizationTask(ldCore::instance()->soundDataProvider(), hwBatch);
    // it's the first instance, set openlase as a global one
    if(m_dataDispatcherInstances.empty())
        ldShape::setGlobalRenderer(task->get_openlase());

    task->moveToThread(taskWorker->thread());
    taskWorker->loadTask(task);
    taskWorker->startTask();

    ldDataDispatcherInstance* ddi = new ldDataDispatcherInstance{dataDispatcher, hwBatch, frameBuffer, taskWorker, task};
    m_dataDispatcherInstances.push_back(ddi);

    if(ldCore::instance()->get_laserController() && ldCore::instance()->get_laserController()->get_isActive())
        dataDispatcher->setActiveTransfer(true);

    connect(dataDispatcher, &ldDataDispatcher::activeXferChanged, this, [&, hwBatch](bool active) {
        if(active)
            QTimer::singleShot(0, hwBatch, &ldHardwareBatch::updateIds); // deadlock otherwise

        emit activeXferChanged(active);
    });

    connect(dataDispatcher, &ldDataDispatcher::activeChanged, this, [&](bool active) {
        emit activeChanged(active);
    });

    emit dataDispatcherCreated(m_dataDispatcherInstances.size()-1);

    update_count(m_count+1);

    return ddi;
}

void ldDataDispatcherManager::deleteDataDispatcher(ldDataDispatcherInstance *dd)
{
    auto it = std::find_if(m_dataDispatcherInstances.begin(), m_dataDispatcherInstances.end(), [&](ldDataDispatcherInstance *eachDd) {
        return eachDd == dd;
    });

    if(it == m_dataDispatcherInstances.end())
        return;


    emit dataDispatcherAboutToDelete(it - m_dataDispatcherInstances.begin());

    m_dataDispatcherInstances.erase(it);

    ldCore::instance()->taskManager()->deleteTaskWorker(dd->taskWorker);
    delete dd->dataDispatcher;
    delete dd->frameBuffer;
    delete dd->hwBatch;

    update_count(m_count-1);

    // update ids of other projectors in case if something was playing with higher priority
    for(ldDataDispatcherInstance* dataDispatcherInstance : m_dataDispatcherInstances)
        dataDispatcherInstance->hwBatch->updateIds();
}

int ldDataDispatcherManager::indexOf(ldDataDispatcherInstance *dd) const
{
    auto it = std::find_if(m_dataDispatcherInstances.begin(), m_dataDispatcherInstances.end(), [&](ldDataDispatcherInstance *eachDd) {
        return eachDd == dd;
    });

    if(it == m_dataDispatcherInstances.end())
        return -1;

    return it - m_dataDispatcherInstances.begin();
}


bool ldDataDispatcherManager::isActiveTransfer() const
{
    for(auto dataDispatcherInstance : m_dataDispatcherInstances)
        return dataDispatcherInstance->dataDispatcher->isActiveTransfer();

    return false;
}

void ldDataDispatcherManager::setActiveTransfer(bool active)
{
    for(auto dataDispatcherInstance : m_dataDispatcherInstances)
        dataDispatcherInstance->dataDispatcher->setActiveTransfer(active);
}

void ldDataDispatcherManager::setActive(bool active)
{
    for(auto dataDispatcherInstance : m_dataDispatcherInstances)
        dataDispatcherInstance->dataDispatcher->setActive(active);

    ldCore::instance()->hardwareManager()->set_isActive(active);
}

ldDataDispatcherInstance *ldDataDispatcherManager::get(int index) const
{
    if(index < 0 || index >= static_cast<int>(m_dataDispatcherInstances.size()))
        return nullptr;

    return m_dataDispatcherInstances[index];
}

