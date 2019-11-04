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
#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterUncreatableType<ldAbstractGame>("WickedLasers", 1, 0, "LdGameState", "LdGameState enum can't be created");
#endif
    ldGamepad::registerMetaTypes();
}

ldAbstractGame::ldAbstractGame(const QString &id, const QString &title, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_title(title)
    , m_gamepadCtrl(new ldGamepadCtrl(this, this))
    , m_isActive(false)
    , m_levelIndex(0)
    , m_state(Ready)
{
#ifdef LD_CORE_ENABLE_QT_QUICK
    qmlRegisterType<ldGamepadCtrl>();
#endif

    connect(this, &ldAbstractGame::levelIndexChanged, this, &ldAbstractGame::onLevelIndexChanged);
    connect(this, &ldAbstractGame::isActiveChanged, this, &ldAbstractGame::onActiveChanged);

#ifdef LD_CORE_GAMES_ALWAYS_PLAY_STATE
    connect(this, &ldAbstractGame::stateChanged, this, [&](int state) {
       if(state == Ready)
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

QString ldAbstractGame::get_levelListName() const
{
    return m_levelListName;
}

QStringList ldAbstractGame::get_keyDescriptions() const
{
    return m_keyDescriptions;
}

bool ldAbstractGame::eventFilter(QObject *obj, QEvent *ev) {
    if(m_state != ldAbstractGame::Playing) {
        return QObject::eventFilter(obj, ev);
    }

    if(ev->type() == QEvent::KeyPress || ev->type() == QEvent::KeyRelease) {
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent*>(ev);
        Q_ASSERT(keyEvent);
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
    if(m_state != Playing)
        toggle();
}

void ldAbstractGame::pause()
{
    if(m_state == Playing)
        toggle();
}

void ldAbstractGame::reset() {
    if(m_state != ldAbstractGame::Ready) {
        toggle();
    }

    getGameVisualizer()->reset();

    set_state(Ready);
}

void ldAbstractGame::toggle() {
    qDebug() << "Game:" << get_title() << "isPlaying changed to" << m_state;

    getGameVisualizer()->togglePlay();

    if(get_state() == Playing)
        set_state(Paused);
    else
        set_state(Playing);
}

bool ldAbstractGame::isSoundEnabled() const
{
    return getGameVisualizer()->isSoundEnabled();
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

