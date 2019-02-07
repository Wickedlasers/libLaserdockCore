#include "ldSpiralFighterGame.h"

#include <QtGui/QKeyEvent>

#include "vis/ldSpiralFighterVisualizer.h"

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
