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

class ldAudioDecoder;
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

/** Base class for laserdock usage. Provides access to control laserdock activation/filters/audio processing/visualizations */
class LDCORESHARED_EXPORT ldCore : public QObject
{
    Q_OBJECT

    /** Helper high-level class for easier laser control */
    QML_READONLY_PROPERTY(ldLaserController*, laserController)

public:
    /** Singleton */
    static ldCore* instance();
    static ldCore* create(QObject *parent);

    /** Init Qt resources. Must be called before QCoreApplication created */
    static void initResources();

    /** destructor */
    virtual ~ldCore();

    /** Initialize ldCore managers. Must be called in order to use it*/
    virtual void initialize();

    /** Path to external resources, like fonts, svgs, etc */
    virtual QString resourceDir() const;

    /** Managers/accessors*/
    ldAudioDecoder *audioDecoder() const;
    ldDataDispatcher *dataDispatcher() const;
    ldBufferManager *bufferManager() const;
    ldFilterManager *filterManager() const;
    ldHardwareManager *hardwareManager() const;
    ldMusicManager *musicManager() const;
    ldRendererManager *rendererManager() const;
    ldSoundDeviceManager *soundDeviceManager() const;
    ldVisualizationTask *task() const;
    ldTaskManager *taskManager() const;

protected:
    explicit ldCore(QObject *parent = 0);

    ldAudioDecoder *m_audioDecoder = nullptr;
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
