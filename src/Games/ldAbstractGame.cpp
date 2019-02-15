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

#include "ldCore/Games/ldAbstractGame.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QtDebug>
#include <QtGui/QtEvents>
#include <QtQml/QQmlEngine>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

void ldAbstractGame::registerMetaTypes()
{
    ldGamepad::registerMetaTypes();
}

ldAbstractGame::ldAbstractGame(const QString &id, const QString &title, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_title(title)
    , m_gamepadCtrl(new ldGamepadCtrl(this, this))
    , m_isActive(false)
    , m_levelIndex(0)
    , m_isPlaying(false)
    , m_isPaused(false)
{
#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterType<ldGamepadCtrl>();
#endif

    connect(this, &ldAbstractGame::levelIndexChanged, this, &ldAbstractGame::onLevelIndexChanged);
    connect(this, &ldAbstractGame::isActiveChanged, this, &ldAbstractGame::onActiveChanged);

#ifdef LD_CORE_GAMES_ALWAYS_PLAY_STATE
    connect(this, &ldAbstractGame::isPlayingChanged, this, [&](bool /*isPlaying*/) {
       if(!m_isPaused)
           play();
    });
#endif

    m_gamepadCtrl->installEventFilter(this);
}

ldAbstractGame::~ldAbstractGame()
{
}

QStringList ldAbstractGame::get_levelList() const
{
    return m_levelList;
}

QStringList ldAbstractGame::get_keyDescriptions() const
{
    return m_keyDescriptions;
}

bool ldAbstractGame::eventFilter(QObject *obj, QEvent *ev) {
    if(!get_isPlaying()) {
        return QObject::eventFilter(obj, ev);
    }

    if(ev->type() == QEvent::KeyPress || ev->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(ev);
        if(keyEvent->modifiers() == Qt::NoModifier
                || keyEvent->modifiers() == Qt::KeypadModifier ) {
            if(handleKeyEvent(keyEvent)) {
                keyEvent->accept();
                return true;
            }
        }
    }

    return QObject::eventFilter(obj, ev);
}

ldAbstractGameVisualizer *ldAbstractGame::getVisualizer() const
{
    return getGameVisualizer();
}

void ldAbstractGame::moveX(double x)
{
    getGameVisualizer()->moveX(x);
}

void ldAbstractGame::moveY(double y)
{
    getGameVisualizer()->moveY(y);
}

void ldAbstractGame::moveRightX(double x)
{
    getGameVisualizer()->moveRightX(x);
}

void ldAbstractGame::moveRightY(double y)
{
    getGameVisualizer()->moveRightY(y);
}

void ldAbstractGame::play()
{
    if(!m_isPlaying)
        toggle();
}

void ldAbstractGame::pause()
{
    if(m_isPlaying && !m_isPaused)
        toggle();
}

void ldAbstractGame::reset() {
    if(get_isPlaying()) {
        toggle();
    }

    getGameVisualizer()->reset();

    set_isPaused(false);
}

void ldAbstractGame::toggle() {
    qDebug() << "Game:" << get_title() << "isPlaying changed to" << !m_isPlaying;

    getGameVisualizer()->togglePlay();

    bool isPlaying = !m_isPlaying;
    set_isPaused(!isPlaying);
    set_isPlaying(isPlaying);
}

void ldAbstractGame::setComplexity(float speed)
{
    getGameVisualizer()->setComplexity(speed);
}

void ldAbstractGame::setSoundEnabled(bool enabled)
{
    getGameVisualizer()->setSoundEnabled(enabled);
}

void ldAbstractGame::setSoundLevel(int soundLevel)
{
    getGameVisualizer()->setSoundLevel(soundLevel);
}

void ldAbstractGame::activate()
{
    //  visualizer
    ldCore::instance()->task()->setCurrentVisualizer(getGameVisualizer());
}

void ldAbstractGame::deactivate()
{
    pause();
}

void ldAbstractGame::onActiveChanged(bool isActive)
{
    if(isActive) {
        activate();
    } else {
        deactivate();
    }
}

void ldAbstractGame::onLevelIndexChanged(int index)
{
    getGameVisualizer()->setLevelIndex(index);
}

