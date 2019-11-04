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

#include "ldCore/Data/ldBufferManager.h"
#include "ldCore/Data/ldDataDispatcher.h"
#include "ldCore/Filter/ldFilterManager.h"
#include "ldCore/Hardware/ldHardwareManager.h"
#include "ldCore/Helpers/Maths/ldVec2.h"
#include "ldCore/Helpers/Text/ldSvgFontManager.h"
#include "ldCore/Helpers/ldLaserController.h"
#include "ldCore/Simulator/ldSimulatorEngine.h"
#include "ldCore/Sound/ldAudioDecoder.h"
#include "ldCore/Sound/ldSoundDeviceManager.h"
#include "ldCore/Task/ldTaskManager.h"
#include "ldCore/Task/ldTaskWorker.h"
#include "ldCore/Task/ldAbstractTask.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

#ifdef LD_CORE_ENABLE_QT_QUICK
#include "ldCore/Simulator/ldSimulatorItem.h"
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

    ldVec2::registerMetaTypes();

#ifdef LD_CORE_ENABLE_QT_QUICK
    ldSimulatorItem::registerMetatypes();
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

    m_hardwareManager = new ldHardwareManager(this);

    m_filterManager = new ldFilterManager();
    m_bufferManager = new ldBufferManager(this);

    m_dataDispatcher = new ldDataDispatcher(m_bufferManager, m_hardwareManager, this);

    // create and load task
    m_taskManager = new ldTaskManager(m_bufferManager, this);
     // filter manager should be on same thread, because task/buffer uses pointers that are owned by filter manager
    m_filterManager->moveToThread(m_taskManager->taskWorker()->thread());

    // music manager
    m_musicManager = new ldMusicManager(this);

    m_soundDeviceManager = new ldSoundDeviceManager(this);

    // load task
    m_task = new ldVisualizationTask(m_musicManager, m_audioDecoder);
    m_task->moveToThread(m_taskManager->taskWorker()->thread());
    m_taskManager->taskWorker()->loadTask(m_task);
    m_taskManager->taskWorker()->startTask();

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

ldDataDispatcher *ldCore::dataDispatcher() const
{
    return m_dataDispatcher;
}

ldBufferManager *ldCore::bufferManager() const
{
    return m_bufferManager;
}

ldFilterManager *ldCore::filterManager() const
{
    return m_filterManager;
}

ldHardwareManager *ldCore::hardwareManager() const
{
    return m_hardwareManager;
}

ldMusicManager *ldCore::musicManager() const
{
    return m_musicManager;
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
    return m_task;
}

ldTaskManager *ldCore::taskManager() const
{
    return m_taskManager;
}

ldCore::ldCore(QObject *parent)
    : QObject(parent)
    , m_laserController(nullptr)
{
#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterType<ldLaserController>();
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
#if defined(Q_OS_MAC)
        m_resourceDir = qApp->applicationDirPath() + "/../Resources";
#elif defined(Q_OS_WIN32)
        m_resourceDir = qApp->applicationDirPath() + "/Resources";
#elif defined(Q_OS_ANDROID)
        m_resourceDir = storageDir() + "/resources";
#else
        m_resourceDir = qApp->applicationDirPath();
#endif
}


