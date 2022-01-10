//
// Laser Angry Lasers
// By Vander R. N. Dias a.k.a. "imerso" @ imersiva.com
// Copyright(c) 2019 Wicked Lasers
//

#include "ldAngryLasers.h"

#include <QtGui/QKeyEvent>

#include "vis/ldAngryLasersVisualizer.h"

ldAngryLasers::ldAngryLasers(QObject *parent)
    : ldAbstractGame("AngryLasersGame", tr("Angry Lasers"), parent)
    , m_visualizer(new ldAngryLasersVisualizer) {

    m_keyDescriptions << "Left" << "Move Left";
    m_keyDescriptions << "Right" << "Move Right";
    m_keyDescriptions << "Up" << "Move Up";
    m_keyDescriptions << "Down" << "Move Down";
    m_keyDescriptions << "Space" << "Fire";

    QMap<ldGamepad::Button, Qt::Key> keyMap;
    keyMap[ldGamepad::Button::Left] = Qt::Key_Left;
    keyMap[ldGamepad::Button::Right] = Qt::Key_Right;
    keyMap[ldGamepad::Button::Up] = Qt::Key_Up;
    keyMap[ldGamepad::Button::Down] = Qt::Key_Down;
    keyMap[ldGamepad::Button::B] = Qt::Key_Space;
    get_gamepadCtrl()->init(keyMap, m_keyDescriptions);
}

ldAngryLasers::~ldAngryLasers() {
}

bool ldAngryLasers::handleKeyEvent(QKeyEvent *keyEvent) {
    if(keyEvent->type() == QEvent::KeyPress) {
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            m_visualizer->onPressedLeft(true);
            break;
        case Qt::Key_Right:
            m_visualizer->onPressedRight(true);
            break;
        case Qt::Key_Up:
            m_visualizer->onPressedUp(true);
            break;
        case Qt::Key_Down:
            m_visualizer->onPressedDown(true);
            break;
        case Qt::Key_Space:
            m_visualizer->onPressedShoot(true);
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
        case Qt::Key_Up:
            m_visualizer->onPressedUp(false);
            break;
        case Qt::Key_Down:
            m_visualizer->onPressedDown(false);
            break;
        case Qt::Key_Space:
            m_visualizer->onPressedShoot(false);
            break;
        default:
            break;
        }
        return true;
    }

    return false;
}

ldAbstractGameVisualizer *ldAngryLasers::getGameVisualizer() const {
    return m_visualizer.data();
}
