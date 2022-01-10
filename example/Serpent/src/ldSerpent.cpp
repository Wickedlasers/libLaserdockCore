#include "ldSerpent.h"

#include <QtGui/QtEvents>

#include "vis/ldSerpentVisualizer.h"

ldSerpent::ldSerpent(QObject *parent)
    : ldAbstractGame("snakeGame", tr("Serpent B"), parent)
    , m_visualizer(new ldSerpentVisualizer) {
    m_keyDescriptions << "Left" << "Left";
    m_keyDescriptions << "Right" << "Right";
    m_keyDescriptions << "Up" << "Up";
    m_keyDescriptions << "Down" << "Down";

    QMap<ldGamepad::Button, Qt::Key> keyMap;
    keyMap[ldGamepad::Button::Left] = Qt::Key_Left;
    keyMap[ldGamepad::Button::Right] = Qt::Key_Right;
    keyMap[ldGamepad::Button::Up] = Qt::Key_Up;
    keyMap[ldGamepad::Button::Down] = Qt::Key_Down;
    get_gamepadCtrl()->init(keyMap, m_keyDescriptions);
}

ldSerpent::~ldSerpent() {
}

bool ldSerpent::handleKeyEvent(QKeyEvent *keyEvent) {
    if(keyEvent->type() == QEvent::KeyPress) {
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            m_visualizer->moveLeft(true);
            break;
        case Qt::Key_Right:
            m_visualizer->moveRight(true);
            break;
        case Qt::Key_Down:
            m_visualizer->moveDown(true);
            break;
        case Qt::Key_Up:
            m_visualizer->moveUp(true);
            break;
        default:
            break;
        }
        return true;
    } else if(keyEvent->type() == QEvent::KeyRelease) {
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            m_visualizer->moveLeft(false);
            break;
        case Qt::Key_Right:
            m_visualizer->moveRight(false);
            break;
        case Qt::Key_Down:
            m_visualizer->moveDown(false);
            break;
        case Qt::Key_Up:
            m_visualizer->moveUp(false);
            break;
        default:
            break;
        }
        return true;
    }
    return false;
}

ldAbstractGameVisualizer *ldSerpent::getGameVisualizer() const {
    return m_visualizer.data();
}
