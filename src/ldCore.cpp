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
#include "ldCore/Helpers/ldLaserController.h"
#ifdef LD_CORE_ENABLE_QT_QUICK
#include "ldCore/Simulator/ldSimulatorItem.h"
#endif
#include "ldCore/Simulator/ldSimulatorEngine.h"
#include "ldCore/Sound/ldAudioDecoder.h"
#include "ldCore/Sound/ldSoundDeviceManager.h"
#include "ldCore/Task/ldTaskManager.h"
#include "ldCore/Task/ldTaskWorker.h"
#include "ldCore/Task/ldAbstractTask.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"


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

    \row
    \li  \l ldRendererManager
    \li  \l ldCore::rendererManager()
    \li  Maintains a list of renderers that can be used to render graphics. Right only only openlase is supported.


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

#ifdef LD_CORE_ENABLE_QT_QUICK
    ldSimulatorItem::registerMetatypes();
#endif
}

/*!
 * \brief removes test dialog and main window upon destruction
 */

ldCore::~ldCore()
{
    m_instance = nullptr;
    delete m_filterManager;
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

    m_hardwareManager = new ldHardwareManager(this);

    m_filterManager = new ldFilterManager();
    m_bufferManager = new ldBufferManager(this);

    m_dataDispatcher = new ldDataDispatcher(m_bufferManager, m_hardwareManager, this);
    m_renderermanager = new ldRendererManager(this);

    // create and load task
    m_taskManager = new ldTaskManager(m_bufferManager, this);
     // filter manager should be on same thread, because task/buffer uses pointers that are owned by filter manager
    m_filterManager->moveToThread(m_taskManager->taskWorker()->thread());

    // music manager
    m_musicManager = new ldMusicManager(this);

    m_soundDeviceManager = new ldSoundDeviceManager(this);

    // load task
    m_task = new ldVisualizationTask;
    m_task->moveToThread(m_taskManager->taskWorker()->thread());
    m_taskManager->taskWorker()->loadTask(m_task);
    m_taskManager->taskWorker()->startTask();

    update_laserController(new ldLaserController(this));

    qDebug() << "Core initialized";
}

QString ldCore::resourceDir() const
{
#if defined(Q_OS_MAC)
    return qApp->applicationDirPath() + "/../Resources";
#elif defined(Q_OS_WIN32)
    return qApp->applicationDirPath() + "/Resources";
#else
    return qApp->applicationDirPath();
#endif
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

ldRendererManager *ldCore::rendererManager() const
{
    return m_renderermanager;
}

ldSoundDeviceManager *ldCore::soundDeviceManager() const
{
    return m_soundDeviceManager;
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
}


