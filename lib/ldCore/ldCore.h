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

#ifndef LDCORE_H
#define LDCORE_H

#include <QQmlHelpers>

#include "ldCore/ldCore_global.h"

class ldBufferManager;
class ldDataDispatcher;
class ldFilterManager;
class ldLaserController;
class ldHardwareManager;
class ldMusicManager;
class ldRendererManager;
class ldSoundDeviceManager;
class ldTaskManager;
class ldVisualizationTask;

class LDCORESHARED_EXPORT ldCore : public QObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(ldLaserController*, laserController)

public:
    static ldCore* instance();

    static bool isDebugMode();

    static void initResources();

    explicit ldCore(QObject *parent = 0);
    virtual ~ldCore();

    virtual void initialize();

    virtual QString resourceDir() const;

    ldDataDispatcher *dataDispatcher() const;
    ldFilterManager *filterManager() const;
    ldHardwareManager *hardwareManager() const;
    ldMusicManager *musicManager() const;
    ldRendererManager *rendererManager() const;
    ldSoundDeviceManager *soundDeviceManager() const;
    ldVisualizationTask *task() const;
    ldTaskManager *taskManager() const;

protected:
    ldBufferManager *m_bufferManager = nullptr;
    ldDataDispatcher *m_dataDispatcher = nullptr;
    ldFilterManager *m_filterManager = nullptr;
    ldHardwareManager *m_hardwareManager = nullptr;
    ldMusicManager *m_musicManager = nullptr;
    ldRendererManager *m_renderermanager = nullptr;
    ldSoundDeviceManager *m_soundDeviceManager = nullptr;
    ldVisualizationTask *m_task = nullptr;
    ldTaskManager *m_taskManager = nullptr;

private:
    static ldCore *m_instance;
};

#endif // LDCORE_H
