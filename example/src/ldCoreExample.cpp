/**
**	libLaserdockCore
**	Copyright(c) 2018 Wicked Lasers
**
**	GNU Lesser General Public License
**	This file may be used under the terms of the GNU Lesser
**  General Public License version 3 as published by the Free
**  Software Foundation and appearing in the file LICENSE.LGPLv3 included
**  in the packaging of this file. Please review the following information
**  to ensure the GNU Lesser General Public License requirements
**  will be met: https://www.gnu.org/licenses/lgpl.html
**
**/

#include "ldCoreExample.h"

#include <QtGui/QGuiApplication>
#include <QtCore/QtDebug>
#include <QtCore/QTimer>
#include <QtQml/QtQml>

#include <ldCore/ldCore.h>
#include <ldCore/Visualizations/ldVisualizationTask.h>

#include "src/Games/ldSpiralFighterGame.h"
#include "src/Visualizations/Visualizers/Animation/ldGoGoGirlAnimationVisualizer.h"
#include "src/Visualizations/Visualizers/Animation/ldGoGoGirlAnimationVisualizer.h"
#include "src/Visualizations/Visualizers/Clock/ldAnalogClockVisualizer.h"
#include "src/Visualizations/Visualizers/Simple/ldCircleVisualizer.h"
#include "src/Visualizations/Visualizers/Simple/ldSquareVisualizer.h"
#include "src/Visualizations/Visualizers/Spectrum/ldSpectrumBandVisualizer.h"

ldCoreExample::ldCoreExample(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent)
    , m_ldCore(new ldCore(parent))
    , m_game(nullptr)
    , m_qmlEngine(engine)
{
    qmlRegisterType<ldCore>();
    qmlRegisterType<ldSpiralFighterGame>();

    m_ldCore->initialize();

    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldCircleVisualizer));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldSquareVisualizer));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldSpectrumBandVisualizer));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldGoGoGirlAnimationVisualizer));
    m_visualizers.push_back(std::unique_ptr<ldVisualizer>(new ldAnalogClockVisualizer));

    m_game = new ldSpiralFighterGame(this);

    QTimer::singleShot(0, this, &ldCoreExample::init);
}

ldCoreExample::~ldCoreExample()
{
}

void ldCoreExample::init()
{
    // init qml
    m_qmlEngine->rootContext()->setContextObject(this);
    m_qmlEngine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if(m_qmlEngine->rootObjects().isEmpty()) {
        QTimer::singleShot(0, qApp, &QGuiApplication::quit);
        return;
    }

    ldCore::instance()->task()->setCurrentVisualizer(m_visualizers[0].get());
}

void ldCoreExample::activateVis(int index)
{
    if(index < 5) {
        ldCore::instance()->task()->setCurrentVisualizer(m_visualizers[index].get());
        m_game->set_isActive(false);
    } else {
        m_game->set_isActive(true);
    }
}

void ldCoreExample::setWindow(QObject *window)
{
    window->installEventFilter(m_game);
}
