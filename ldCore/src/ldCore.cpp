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

#include "ldCore/ldCore.h"

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtQml/QtQml>
#include "ldCore/Data/ldDataDispatcher.h"
#include "ldCore/Data/ldDataDispatcherManager.h"
#include <ldCore/Data/ldFrameBuffer.h>
#include "ldCore/Filter/ldFilterManager.h"
#include <ldCore/Games/ldAbstractGame.h>
#include "ldCore/Hardware/ldHardwareBatch.h"
#include "ldCore/Hardware/ldHardwareManager.h"
#include "ldCore/Hardware/ldNetworkHardwareManager.h"
#include "ldCore/Hardware/ldUSBHardwareManager.h"
#include "ldCore/Helpers/Maths/ldVec2.h"
#include "ldCore/Helpers/Text/ldTextDirection.h"
#include "ldCore/Helpers/Text/ldSvgFontManager.h"
#include "ldCore/Helpers/ldLaserController.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"
#include "ldCore/Sound/ldAudioDecoder.h"
#include "ldCore/Sound/ldSoundDataProvider.h"
#include "ldCore/Sound/ldSoundDeviceManager.h"
#include "ldCore/Task/ldTaskManager.h"
#include "ldCore/Task/ldTaskWorker.h"
#include "ldCore/Task/ldAbstractTask.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

#ifdef LD_CORE_ENABLE_QT_QUICK
#include <ldCore/Simulator/ldQSGSimulatorItem.h>
#endif

#ifdef LD_CORE_ENABLE_OPENGL_SIMULATOR
#include "ldCore/Simulator/ldSimulatorItem.h"
#endif

#ifdef LD_CORE_ENABLE_QUAZIP
#include <ldCore/Android/ldZipExtractor.h>
#endif

#ifdef LD_CORE_RESOURCES_EXTRACTOR
#include <ldCore/Android/ldResourcesExtractor.h>
#endif
/*!

  \class ldCore
  \brief Starting point for the laserdock Application. This is the class that bootstraps everything.

  \inmodule AppCommandAndStateManagement

  The role of ldCore is the starting and reference point of Laserdock application.

  When the application first launches, one instance of this class is created and saved to a global variable
   declared as extern ldCore *ldmgr(). This singular object then proceeds to bootstrap all the components
   neccessary for the execution of the laserdock application.

  Throughout the lifetime of the application, the global function ldmgr() is visible across most classes on the project.
  Various Componenents of the system all refer to this instance of \l ldCore in request for resoruces.
  You can see this in action, for example, in \l ldVisualizationTask::start(), where it tries to acquire
  audio resources before execution of the task.

    The components that are attached to this class are.

    \table
    \header
    \li component class
    \li getter
    \li description

    \row
    \li  \l ldBufferManager
    \li  \l ldCore::bufferManager()
    \li  Responsible for the singlular buffer object that is used to carry data within the application.


    \row
    \li  \l ldTaskManager
    \li  \l ldCore::taskManager()
    \li  Executes and manages task for the application.

    \endtable

  Laserdock project has done away from singleton classes in preference of a singular global variable, due to
  complexities involved with creation and management of singleton classes. All critical resources that require sharing,
  such as buffer, media input, datadispatcher, task execution are instead attached to this class, which handles
  creation, management and destructions of these resources, offering a singular point of reference.

*/

ldCore* ldCore::m_instance = nullptr;

ldCore *ldCore::instance()
{
    return m_instance;
}

ldCore *ldCore::create(QObject *parent)
{
    return new ldCore(parent);
}

void ldCore::initResources()
{
    Q_INIT_RESOURCE(laserdock);

    qRegisterMetaType<ldAbstractTask*>();
    qRegisterMetaType<ldSimulatorEngine*>();

    ldAbstractGame::registerMetaTypes();
    ldVec2::registerMetaTypes();
    ldVertex::registerMetaTypes();
    ldMusicManager::registerMetaTypes();
#ifdef LD_CORE_ENABLE_QT_QUICK
    ldQSGSimulatorItem::registerMetatypes();
#endif
#ifdef LD_CORE_ENABLE_OPENGL_SIMULATOR
    ldSimulatorItem::registerMetatypes();
#endif

#ifdef LD_CORE_ENABLE_QUAZIP
    ldZipExtractor::staticInit();
#endif
}

/*!
 * \brief removes test dialog and main window upon destruction
 */

ldCore::~ldCore()
{
    delete m_filterManager;
    m_instance = nullptr;

    qDebug() << "ldCore destroyed";
}

/*!
    \brief Instantiates member components and set up signals/slot connections.

    This function should be called immediately after the construction of \l ldCore, because the contruction does not completely initialize this class for use.

*/
void ldCore::initialize()
{
    qDebug() << "Core initialize...";

    // create managers
    m_audioDecoder = new ldAudioDecoder(this);

    m_svgFontManager = new ldSvgFontManager(this);
    m_svgFontManager->addFont(ldSvgFont("Roboto", "roboto"));

    m_filterManager = new ldFilterManager();

    m_hardwareManager = new ldHardwareManager(this);
    m_hardwareManager->addHardwareManager(new ldNetworkHardwareManager(m_filterManager));
#ifdef LASERDOCKLIB_USB_SUPPORT
    m_hardwareManager->addHardwareManager(new ldUsbHardwareManager(m_filterManager, this));
#endif
    m_hardwareManager->set_isActive(true);

    // music manager
    m_musicManager = new ldMusicManager(this);
    m_soundDeviceManager = new ldSoundDeviceManager(this);
    m_soundDataProvider = new ldSoundDataProvider(m_musicManager, m_audioDecoder, this);
    m_soundDataProvider->start();

    m_taskManager = new ldTaskManager(this);

    m_dataDispatcherManager = new ldDataDispatcherManager(this);
    m_dataDispatcherInstance.reset(m_dataDispatcherManager->createNew());
    m_dataDispatcherInstance->hwBatch->setDefaultMode(true);
    connect(qApp, &QCoreApplication::aboutToQuit, [&]() {
        m_dataDispatcherManager->deleteDataDispatcher(m_dataDispatcherInstance.get());
    });

    // filter manager should be on same thread, because task/buffer uses pointers that are owned by filter manager
    // NOTE: do we really need it? all calls are done from various threads
//    m_filterManager->moveToThread(taskWorker->thread());

    update_dataDispatcher(m_dataDispatcherInstance->dataDispatcher);

    update_laserController(new ldLaserController(this));

#ifdef LD_CORE_RESOURCES_EXTRACTOR
    m_resourcesExtractor = new ldResourcesExtractor(this);
#endif

    qDebug() << "Core initialized";
}

QString ldCore::storageDir() const
{
    return m_storageDir;
}

void ldCore::setStorageDir(const QString &storageDir)
{
    m_storageDir = storageDir;
}

QString ldCore::resourceDir() const
{
    return m_resourceDir;
}

void ldCore::setResourceDir(const QString &resourceDir)
{
    m_resourceDir = resourceDir;
}

ldAudioDecoder *ldCore::audioDecoder() const
{
    return m_audioDecoder;
}

ldFilterManager *ldCore::filterManager() const
{
    return m_filterManager;
}

ldHardwareBatch *ldCore::hwBatch() const
{
    return m_dataDispatcherInstance->hwBatch;
}

ldHardwareManager *ldCore::hardwareManager() const
{
    return m_hardwareManager;
}

ldMusicManager *ldCore::musicManager() const
{
    return m_musicManager;
}

ldSoundDataProvider *ldCore::soundDataProvider() const
{
    return m_soundDataProvider;
}

ldSoundDeviceManager *ldCore::soundDeviceManager() const
{
    return m_soundDeviceManager;
}

ldSvgFontManager *ldCore::svgFontManager() const
{
    return m_svgFontManager;
}

ldVisualizationTask *ldCore::task() const
{
    return m_dataDispatcherInstance->task;
}

ldTaskManager *ldCore::taskManager() const
{
    return m_taskManager;
}

ldCore::ldCore(QObject *parent)
    : QObject(parent)
    , m_dataDispatcher(nullptr)
    , m_laserController(nullptr)
{
#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterAnonymousType<ldDataDispatcher>("WickedLasers", 1);
    qmlRegisterAnonymousType<ldDataDispatcherManager>("WickedLasers", 1);
    qmlRegisterAnonymousType<ldLaserController>("WickedLasers", 1);
#endif

    m_instance = this;

    initStorageDir();
    // depends on storage dir on android, initialize second!
    initResourceDir();
}

void ldCore::initStorageDir()
{
    QString storageLocation;

    QStringList appDataLocations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
    if(!appDataLocations.isEmpty()) {
#ifdef Q_OS_ANDROID
        // store in external storage on Android, simple last() works
        storageLocation = appDataLocations.last();
#else
        // default on other systems
        storageLocation = appDataLocations.first();
#endif
    } else {
        // probably won't work, but at least we tried
        storageLocation = qApp->applicationDirPath();
        qWarning() << "Can't locate QStandardPaths::AppDataLocation. Use app dir";
    }

    m_storageDir = storageLocation;
}

void ldCore::initResourceDir()
{
#if defined(Q_OS_MACOS)
        m_resourceDir = QDir(qApp->applicationDirPath() + "/../Resources").absolutePath();
#elif defined(Q_OS_WIN32)
        m_resourceDir = qApp->applicationDirPath() + "/Resources";
#elif defined(Q_OS_ANDROID)
        m_resourceDir = storageDir() + "/resources";
#elif defined(Q_OS_IOS)
        m_resourceDir = qApp->applicationDirPath() + "/Res";
#else
        m_resourceDir = qApp->applicationDirPath();
#endif
}


