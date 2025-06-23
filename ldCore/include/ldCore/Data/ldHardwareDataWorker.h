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

#ifndef ldHardwareDataWorker_H
#define ldHardwareDataWorker_H

#include <QtCore/QThread>
#include <QtCore/QEventLoop>

#include "ldAbstractDataWorker.h"

class ldFrameBuffer;
class ldAbstractHardwareManager;
class ldHardwareBatch;
class ldSimulatorEngine;
class ldThreadedDataWorker;

/** USB data worker */
class LDCORESHARED_EXPORT ldHardwareDataWorker : public ldAbstractDataWorker
{
    Q_OBJECT
public:
    /** Constructor/destructor */
    explicit ldHardwareDataWorker(ldFrameBuffer *frameBuffer,
                              ldHardwareBatch *hardwareBatch,
                              ldSimulatorEngine *simulatorEngine,
                              QObject *parent = nullptr);
    ~ldHardwareDataWorker();

    /** ldAbstractDataWorker implementations */
    virtual bool isActive() const override;
    virtual bool isActiveTransfer() const override;

public slots:
    /** ldAbstractDataWorker implementations */
    virtual void setActive(bool active) override;
    virtual void setActiveTransfer(bool active) override;

    /** Flag to use this worker to fill simulator buffer */
    void setSimulatorEnabled(bool enabled);

private slots:
    void stop();

private:
    bool m_isActive = false;

    QScopedPointer<ldThreadedDataWorker> m_thread_worker;
    QThread m_worker_thread;

    ldHardwareBatch *m_hwBatch;
    ldSimulatorEngine* m_simulatorEngine;
};

#endif /* ldHardwareDataWorker_H */
