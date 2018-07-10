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

#include "ldAbstractGame.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QtDebug>
#include <QtGui/QtEvents>
#include <QtQml/QQmlEngine>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Visualizations/Visualizers/Games/ldAbstractGameVisualizer.h"

#ifdef TOUCH_KEY_SUPPORT
void ldAbstractGame::registerMetaTypes()
{
    qmlRegisterUncreatableType<ldAbstractGame>("WickedLasers", 1, 0, "LdGameApp", "LdGameApp enum can't be created");
}
#endif

ldAbstractGame::ldAbstractGame(const QString &id, const QString &title, QObject *parent)
    : QObject(parent)
    , m_id(id)
    , m_title(title)
    , m_isActive(false)
    , m_levelIndex(0)
    , m_isPlaying(false)
    , m_isPaused(false)
{
    connect(this, &ldAbstractGame::levelIndexChanged, this, &ldAbstractGame::onLevelIndexChanged);
    connect(this, &ldAbstractGame::isActiveChanged, this, &ldAbstractGame::onActiveChanged);

#ifdef ALWAYS_PLAY_STATE
    connect(this, &ldAbstractGame::isPlayingChanged, this, [&](bool /*isPlaying*/) {
       if(!m_isPaused)
           play();
    });
#endif


#ifdef TOUCH_KEY_SUPPORT
    installEventFilter(this);
#endif
}

ldAbstractGame::~ldAbstractGame()
{
}

QStringList ldAbstractGame::get_levelList() const
{
    return m_levelList;
}

QStringList ldAbstractGame::get_hotkeys() const
{
    return m_hotkeys;
}

#ifdef TOUCH_KEY_SUPPORT
QStringList ldAbstractGame::get_touchHotkeys() const
{
    return m_touchHotkeys;
}
#endif

void ldAbstractGame::play()
{
    if(!m_isPlaying)
        toggle();
}

void ldAbstractGame::pause()
{
    if(m_isPlaying)
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

#ifdef TOUCH_KEY_SUPPORT
void ldAbstractGame::sendKeyEvent(Qt::Key key, bool isPressed)
{
    QEvent::Type eventType = isPressed ? QEvent::KeyPress : QEvent::KeyRelease;
    QKeyEvent *eve1 = new QKeyEvent (eventType, key, Qt::NoModifier, "");

    qApp->postEvent(this, eve1);
}

int ldAbstractGame::getKey(int key)
{
    return m_keyMap[TouchKey(key)];
}

void ldAbstractGame::initTouchHotkeys()
{
    for(const TouchKey &touchKey : m_keyMap.keys()) {
        QString touchKeyString;
        switch (touchKey) {
        case TouchKey::Left:
            touchKeyString = tr("Left");
            break;
        case TouchKey::Right:
            touchKeyString = tr("Right");
            break;
        case TouchKey::Up:
            touchKeyString = tr("Up");
            break;
        case TouchKey::Down:
            touchKeyString = tr("Down");
            break;
        case TouchKey::A:
            touchKeyString = tr("A");
            break;
        case TouchKey::B:
            touchKeyString = tr("B");
            break;
        case TouchKey::X:
            touchKeyString = tr("X");
            break;
        case TouchKey::Y:
            touchKeyString = tr("Y");
            break;
        }


        QString mappedValue;
        Qt::Key mappedKey = m_keyMap[touchKey];
        QString mappedKeyString = QKeySequence(mappedKey).toString();
        for(int i = 0; i < m_hotkeys.length(); i+=2) {
            QString desktopKeyString = m_hotkeys[i];
            // key is mapped directly
            bool isEquals = (desktopKeyString.compare(mappedKeyString, Qt::CaseInsensitive) == 0);
            // Any key?
            bool isAnyKey = (desktopKeyString == "Any Key");
            // some games have multiple keys for single action, divided by "/" (e.g. Laserbeats aka GrooveCoaster)
            QStringList keyList = desktopKeyString.split("/");
            bool isContains = keyList.contains(mappedKeyString, Qt::CaseInsensitive);
            if(isAnyKey || isEquals || isContains) {
                mappedValue = m_hotkeys[i+1];
                break;
            }
        }

        m_touchHotkeys << touchKeyString << mappedValue;
    }
}
#endif

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

