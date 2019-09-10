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

#include "ldCore/Games/ldGamepadCtrl.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QtDebug>
#include <QtGui/QtEvents>
#include <QtQml/QQmlEngine>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/ldVisualizationTask.h"
#include "ldCore/Games/ldAbstractGame.h"

ldGamepadCtrl::ldGamepadCtrl(ldAbstractGame *game, QObject *parent)
    : QObject(parent)
    , m_game(game)
{
}

ldGamepadCtrl::~ldGamepadCtrl()
{
}

void ldGamepadCtrl::init(const QMap<ldGamepad::Button, Qt::Key> &buttonKeyMap, const QStringList &keyDescriptions)
{
    m_buttonKeyMap = buttonKeyMap;

    for(const ldGamepad::Button &button : m_buttonKeyMap.keys()) {
        QString buttonString = buttonToString(button);
        QString buttonDescription = lookForButtonDescription(button, keyDescriptions);

        if(!buttonDescription.isEmpty())
            m_buttonDescriptions << buttonString << buttonDescription;
    }
}

QStringList ldGamepadCtrl::get_buttonDescriptions() const
{
    return m_buttonDescriptions;
}

void ldGamepadCtrl::pressButton(ldGamepad::Button button, bool isPressed)
{
    if(isPressed) {
        // start/select buttons toggle gamplay
        if(button == ldGamepad::Start || button == ldGamepad::Select) {
            m_game->toggle();
            return;
        }

        // any button activate game if it is in paused or game over state
        if(!m_game->get_isPlaying() || m_game->get_isPaused()) {
            m_game->toggle();
            return;
        }

    }

    // otherwise send key to game
    Qt::Key key = m_buttonKeyMap[button];

    QEvent::Type eventType = isPressed ? QEvent::KeyPress : QEvent::KeyRelease;
    QKeyEvent *event = new QKeyEvent (eventType, key, Qt::NoModifier, "");

    qApp->postEvent(this, event);
}

int ldGamepadCtrl::getKey(int button)
{
    QList<ldGamepad::Button> gameButtons = getButtonsFromInt(button);

    for(const ldGamepad::Button &gameButton : gameButtons) {
        if(m_buttonKeyMap.contains(gameButton))
            return m_buttonKeyMap[gameButton];
    }

    return -1;
}

QList<ldGamepad::Button> ldGamepadCtrl::getButtonsFromInt(int button)
{
    QList<ldGamepad::Button> gameButtons;

    if(button & ldGamepad::Button::Up) {
        gameButtons.push_back(ldGamepad::Button::Up);
    }
    if(button & ldGamepad::Button::Left) {
        gameButtons.push_back(ldGamepad::Button::Left);
    }
    if(button & ldGamepad::Button::Right) {
        gameButtons.push_back(ldGamepad::Button::Right);
    }
    if(button & ldGamepad::Button::Down) {
        gameButtons.push_back(ldGamepad::Button::Down);
    }

    if(gameButtons.isEmpty())
        gameButtons.push_back(ldGamepad::Button(button));

//    qDebug() << __FUNCTION__ << button << gameButtons;

    return gameButtons;
}

QString ldGamepadCtrl::buttonToString(const ldGamepad::Button &button) const
{
    QString buttonString;
    switch (button) {
    case ldGamepad::Button::No:
        buttonString = "";
        break;
    case ldGamepad::Button::Left:
        buttonString = tr("Left");
        break;
    case ldGamepad::Button::Right:
        buttonString = tr("Right");
        break;
    case ldGamepad::Button::Up:
        buttonString = tr("Up");
        break;
    case ldGamepad::Button::Down:
        buttonString = tr("Down");
        break;
    case ldGamepad::Button::A:
        buttonString = tr("A");
        break;
    case ldGamepad::Button::B:
        buttonString = tr("B");
        break;
    case ldGamepad::Button::X:
        buttonString = tr("X");
        break;
    case ldGamepad::Button::Y:
        buttonString = tr("Y");
        break;
    case ldGamepad::Button::Start:
        buttonString = tr("Start");
        break;
    case ldGamepad::Button::Select:
        buttonString = tr("Select");
        break;
    }

    return buttonString;
}

QString ldGamepadCtrl::lookForButtonDescription(const ldGamepad::Button &button, const QStringList &keyDescriptions) const
{
    QString mappedValue;
    Qt::Key mappedKey = m_buttonKeyMap[button];
    QString mappedKeyString = QKeySequence(mappedKey).toString();
    for(int i = 0; i < keyDescriptions.length(); i+=2) {
        // check range
        if((i+1) >= keyDescriptions.length())
                break;

        QString desktopKeyString = keyDescriptions[i];
        // key is mapped directly
        bool isEquals = (desktopKeyString.compare(mappedKeyString, Qt::CaseInsensitive) == 0);
        // Any key?
        bool isAnyKey = (desktopKeyString == "Any Key");
        // some games have multiple keys for single action, divided by "/" (e.g. Laserbeats aka GrooveCoaster)
        QStringList keyList = desktopKeyString.split("/");
        bool isContains = keyList.contains(mappedKeyString, Qt::CaseInsensitive);
        if(isAnyKey || isEquals || isContains) {
            mappedValue = keyDescriptions[i+1];
                break;
        }
    }
    return mappedValue;
}
