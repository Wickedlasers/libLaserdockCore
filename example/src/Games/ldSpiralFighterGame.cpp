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

#include "ldSpiralFighterGame.h"

#include <QtGui/QKeyEvent>

#include "src/Visualizations/Visualizers/Games/ldSpiralFighterVisualizer.h"

ldSpiralFighterGame::ldSpiralFighterGame(QObject *parent)
    : ldAbstractGame("spiralFighterGame", tr("Spiral Fighter"), parent)
    , m_visualizer(new ldSpiralFighterVisualizer) {
    m_keyDescriptions << "Left" << "Rotate Left";
    m_keyDescriptions << "Right" << "Rotate Right";
    m_keyDescriptions << "Space" << "Fire";
    m_keyDescriptions << "Enter" << "Power-up";

    QMap<ldGamepad::Button, Qt::Key> keyMap;
    keyMap[ldGamepad::Button::Left] = Qt::Key_Left;
    keyMap[ldGamepad::Button::Right] = Qt::Key_Right;
    keyMap[ldGamepad::Button::A] = Qt::Key_Enter;
    keyMap[ldGamepad::Button::B] = Qt::Key_Space;
    get_gamepadCtrl()->init(keyMap, m_keyDescriptions);

    connect(m_visualizer.data(), &ldSpiralFighterVisualizer::finished, this, [&]() {
       set_isPaused(false);
       set_isPlaying(false);
    });
}

ldSpiralFighterGame::~ldSpiralFighterGame() {
}

bool ldSpiralFighterGame::handleKeyEvent(QKeyEvent *keyEvent) {
    if(keyEvent->type() == QEvent::KeyPress) {
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            m_visualizer->onPressedLeft(true);
            break;
        case Qt::Key_Right:
            m_visualizer->onPressedRight(true);
            break;
        case Qt::Key_Space:
            m_visualizer->onPressedShoot(true);
            break;
        case Qt::Key_Return:
            m_visualizer->onPressedPowerup(true);
            break;
        default:
            break;
        }
        return true;
    } else if(keyEvent->type() == QEvent::KeyRelease) {
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            m_visualizer->onPressedLeft(false);
            break;
        case Qt::Key_Right:
            m_visualizer->onPressedRight(false);
            break;
        case Qt::Key_Space:
            m_visualizer->onPressedShoot(false);
            break;
        case Qt::Key_Return:
            m_visualizer->onPressedPowerup(false);
            break;
        default:
            break;
        }
        return true;
    }

    return false;
}

ldAbstractGameVisualizer *ldSpiralFighterGame::getGameVisualizer() const {
    return m_visualizer.data();
}
