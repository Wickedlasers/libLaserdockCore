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

#include <QQmlHelpers>

#include <ldCore/Helpers/ldPropertyObject.h>

class ldFrameBuffer;
class ldHardwareBatch;
class ldSimulatorEngine;
class ldHardwareDataWorker;

/** Laserdock data transfer control class. Supports OpenGL simulator and additional custom worker for internal usage */
class LDCORESHARED_EXPORT ldDataDispatcher : public ldPropertyObject
{
    Q_OBJECT

public:
    /** Constructor/destructor */
    explicit ldDataDispatcher(ldFrameBuffer *frameBuffer,
                              ldHardwareBatch *hardwareBatch,
                              QObject *parent = nullptr);
    ~ldDataDispatcher();

    /** If any worker is active */
    bool isActiveTransfer() const;

    /** Simulator core engine class */
    ldSimulatorEngine* simulatorEngine() const;

public slots:
    /** Activate/deactivate data workers */
    void setActiveTransfer(bool active);
    void setActive(bool active);
signals:
    /** Emitted when data workers state changed */
    void activeXferChanged(bool active);
    void activeChanged(bool active);

private:
    std::unique_ptr<ldSimulatorEngine> m_simulatorEngine;
    std::unique_ptr<ldHardwareDataWorker> m_dataWorker;
};

#endif // LDDATADISPATCHER_H
