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

#ifndef LDDATADISPATCHERPROVIDER_H
#define LDDATADISPATCHERPROVIDER_H

#include <memory>

#include <QQmlHelpers>

#include <ldCore/Helpers/ldPropertyObject.h>

class ldDataDispatcher;
class ldFrameBuffer;
class ldTaskWorker;
class ldHardwareBatch;
class ldVisualizationTask;

struct LDCORESHARED_EXPORT ldDataDispatcherInstance {
    ldDataDispatcher* dataDispatcher;
    ldHardwareBatch* hwBatch;
    ldFrameBuffer* frameBuffer;
    ldTaskWorker* taskWorker;
    ldVisualizationTask* task;
};

/**
 * The main class to manage multiple laser outputs and dispatchers.
 * TODO:
 *  make it thread safe, crashes are possible :*(
 *  especially get() func looks bad here
 */
class LDCORESHARED_EXPORT ldDataDispatcherManager : public ldPropertyObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(int, count)

    QML_READONLY_PROPERTY(QList<int>, fpsList)

public:
    explicit ldDataDispatcherManager(QObject *parent = nullptr);

    ldDataDispatcherInstance *createNew(const QStringList &ids = QStringList());
    void deleteDataDispatcher(ldDataDispatcherInstance *dd);
    int indexOf(ldDataDispatcherInstance* dd) const;

    bool isActiveTransfer() const;
    void setActiveTransfer(bool active);

    void setActive(bool active);

    ldDataDispatcherInstance* get(int index) const;

signals:
    void activeXferChanged(bool active);
    void activeChanged(bool active);

    void dataDispatcherCreated(int index);
    void dataDispatcherAboutToDelete(int index);

    void requestDdActivation(int index);

private:
    void updateFpsList();

    std::vector<ldDataDispatcherInstance*> m_dataDispatcherInstances;
};

#endif // LDDATADISPATCHERPROVIDER_H
