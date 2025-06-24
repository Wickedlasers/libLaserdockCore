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

#include "ldCoreExample.h"

#include <QtGui/QGuiApplication>
#include <QtCore/QtDebug>
#include <QtCore/QTimer>
#include <QtQml/QtQml>

#include <ldCore/Sound/ldSoundDeviceManager.h>
#include <ldCore/Visualizations/ldVisualizationTask.h>


#ifdef LD_CORE_RESOURCES_EXTRACTOR
#include <ldCore/Android/ldResourcesExtractor.h>

#ifndef LD_EXAMPLE_PACKAGE_NAME
#error LD_EXAMPLE_PACKAGE_NAME should be defined
#endif

#ifndef LD_EXAMPLE_RESOURCES_VERSION_CODE
#error LD_EXAMPLE_RESOURCES_VERSION_CODE should be defined
#endif

#endif // LD_CORE_RESOURCES_EXTRACTOR


#include "src/Visualizations/Visualizers/Animation/ldAnimationVisualizer.h"
#include "src/Visualizations/Visualizers/Clock/ldAnalogClockVisualizer.h"
#include "src/Visualizations/Visualizers/Simple/ldCircleVisualizer.h"
#include "src/Visualizations/Visualizers/Simple/ldSquareVisualizer.h"
#include "src/Visualizations/Visualizers/Spectrum/ldSpectrumBandVisualizer.h"
#include "ldCore/Data/ldDataDispatcher.h"

ldCoreExample::ldCoreExample(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent)
    , m_ldCore(ldCore::create(parent))
    , m_game(nullptr)
    , m_qmlEngine(engine)
    #ifdef LD_CORE_RESOURCES_EXTRACTOR
    , m_resExtractor(new ldResourcesExtractor(this))
    #endif
{
    qmlRegisterAnonymousType<ldCore>("WickedLasers", 1);
    qmlRegisterAnonymousType<ldSpiralFighterGame>("WickedLasers", 1);

    m_ldCore->initialize();
    m_ldCore->task()->setIsShowLogo(false);
    m_ldCore->soundDeviceManager()->setDeviceInfo(m_ldCore->soundDeviceManager()->getDefaultDevice(ldSoundDeviceInfo::Type::QAudioInput));

#ifdef LD_CORE_RESOURCES_EXTRACTOR
    m_resExtractor->init(LD_EXAMPLE_PACKAGE_NAME, LD_EXAMPLE_RESOURCES_VERSION_CODE, -1);
    connect(m_resExtractor, &ldResourcesExtractor::finished, this, &ldCoreExample::startApp);
    if(m_resExtractor->get_fileCount() > 0)
        m_resExtractor->startExtraction();
    else
#endif

        startApp();
}

ldCoreExample::~ldCoreExample()
{
}

void ldCoreExample::init()
{
    // init qml
    m_qmlEngine->rootContext()->setContextObject(this);
    m_qmlEngine->load(QUrl(QStringLiteral("qrc:/res/qml/main.qml")));
    if(m_qmlEngine->rootObjects().isEmpty()) {
        QTimer::singleShot(0, qApp, &QGuiApplication::quit);
        return;
    }

    ldCore::instance()->task()->setVisualizer(m_visualizers[0].get());
}

bool ldCoreExample::eventFilter(QObject *obj, QEvent *ev)
{
    if(m_game && m_game->get_isActive())
        return m_game->eventFilter(obj, ev);

    return QObject::eventFilter(obj, ev);
}

void ldCoreExample::activateVis(int index)
{
    if(index < 5) {
        ldCore::instance()->task()->setVisualizer(m_visualizers[index].get());
        update_game(nullptr);
    } else if(index == 5) {
        update_game(m_spiralFigtherGame);
    } else if(index == 6) {
        update_game(m_angryLasersGame);
    } else if(index == 7) {
        update_game(m_arrowGame);
    } else if(index == 8) {
        update_game(m_serpentGame);
    }else if(index == 9) {
        update_game(m_pongGame);
    }
}

void ldCoreExample::setWindow(QObject *window)
{
    window->installEventFilter(this);
}

void ldCoreExample::startApp()
{
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldCircleVisualizer));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldSquareVisualizer));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldSpectrumBandVisualizer));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldAnimationVisualizer(ldCore::instance()->resourceDir() + "/ldva2/Go-Go Girl.ldva2")));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldAnalogClockVisualizer));

    m_angryLasersGame = new ldAngryLasers(this);
    m_arrowGame = new ldArrow(this);
    m_pongGame = new ldPong(this);
    m_serpentGame = new ldSerpent(this);
    m_spiralFigtherGame = new ldSpiralFighterGame(this);

    connect(this, &ldCoreExample::gameChanged, this, [&](ldAbstractGame *game) {
        m_angryLasersGame->set_isActive(false);
        m_arrowGame->set_isActive(false);
        m_pongGame->set_isActive(false);
        m_serpentGame->set_isActive(false);
        m_spiralFigtherGame->set_isActive(false);
        if(game)
            game->set_isActive(true);
    });
    QTimer::singleShot(0, this, &ldCoreExample::init);
}
