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
class ldDataDispatcher;
struct ldDataDispatcherInstance;
class ldDataDispatcherManager;
class ldFilterManager;
class ldLaserController;
class ldHardwareBatch;
class ldHardwareManager;
class ldMusicManager;
class ldSoundDataProvider;
class ldSoundDeviceManager;
class ldSvgFontManager;
class ldTaskManager;
class ldVisualizationTask;

#ifdef LD_CORE_RESOURCES_EXTRACTOR
class ldResourcesExtractor;
#endif


/** Base class for laserdock usage. Provides access to control laserdock activation/filters/audio processing/visualizations */
class LDCORESHARED_EXPORT ldCore : public QObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(ldDataDispatcher*, dataDispatcher)
    QML_READONLY_PROPERTY(ldDataDispatcherManager*, dataDispatcherManager)

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
    void initialize();

    /** Path to storage location */
    QString storageDir() const;
    void setStorageDir(const QString &storageDir);

    /** Path to external resources, like fonts, svgs, etc */
    QString resourceDir() const;
    void setResourceDir(const QString &resourceDir);

    /** Managers/accessors*/
    ldAudioDecoder *audioDecoder() const;
    ldFilterManager *filterManager() const;
    ldHardwareBatch *hwBatch() const;
    ldHardwareManager *hardwareManager() const;
    ldMusicManager *musicManager() const;
    ldSoundDataProvider *soundDataProvider() const;
    ldSoundDeviceManager *soundDeviceManager() const;
    ldSvgFontManager *svgFontManager() const;
    ldVisualizationTask *task() const;
    ldTaskManager *taskManager() const;

#ifdef LD_CORE_RESOURCES_EXTRACTOR
    ldResourcesExtractor* resourcesExtractor() const { return  m_resourcesExtractor; }
#endif

private:
    static ldCore *m_instance;

    explicit ldCore(QObject *parent = nullptr);

    void initStorageDir();
    void initResourceDir();

    QString m_storageDir;
    QString m_resourceDir;
    ldAudioDecoder *m_audioDecoder = nullptr;
    std::unique_ptr<ldDataDispatcherInstance> m_dataDispatcherInstance;
    ldFilterManager *m_filterManager = nullptr;
    ldHardwareManager *m_hardwareManager = nullptr;
    ldMusicManager *m_musicManager = nullptr;
    ldSoundDataProvider *m_soundDataProvider = nullptr;
    ldSoundDeviceManager *m_soundDeviceManager = nullptr;
    ldSvgFontManager *m_svgFontManager = nullptr;
    ldTaskManager *m_taskManager = nullptr;
#ifdef LD_CORE_RESOURCES_EXTRACTOR
    ldResourcesExtractor* m_resourcesExtractor = nullptr;
#endif
};

#endif // LDCORE_H
