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
    m_hotkeys << "Left" << "Rotate Left";
    m_hotkeys << "Right" << "Rotate Right";
    m_hotkeys << "Space" << "Fire";
    m_hotkeys << "Enter" << "Power-up";

#ifdef TOUCH_KEY_SUPPORT
    m_keyMap[TouchKey::Left] = Qt::Key_Left;
    m_keyMap[TouchKey::Right] = Qt::Key_Right;
    m_keyMap[TouchKey::A] = Qt::Key_Enter;
    m_keyMap[TouchKey::B] = Qt::Key_Space;
    initTouchHotkeys();
#endif

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
