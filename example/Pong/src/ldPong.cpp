#include "ldPong.h"

#include <QtGui/QtEvents>

#include "vis/ldPongVisualizer.h"

ldPong::ldPong(QObject *parent)
    : ldAbstractGame("pongGame", tr("Tennis"), parent)
    , m_visualizer(new ldPongVisualizer)
{
    m_keyDescriptions << "Up" << "Up";
    m_keyDescriptions << "Down" << "Down";

    QMap<ldGamepad::Button, Qt::Key> keyMap;
    keyMap[ldGamepad::Button::Up] = Qt::Key_Up;
    keyMap[ldGamepad::Button::Down] = Qt::Key_Down;
    get_gamepadCtrl()->init(keyMap, m_keyDescriptions);
}

ldPong::~ldPong()
{
}

bool ldPong::handleKeyEvent(QKeyEvent *keyEvent)
{
    if(keyEvent->type() == QEvent::KeyPress) {
        switch (keyEvent->key()) {
        case Qt::Key_W:
            m_visualizer->move1Up(true);
            // player 1 up key
            break;
        case Qt::Key_S:
            // player 1 down key
            m_visualizer->move1Down(true);
            break;
        case Qt::Key_Up:
            // player 2 up key
            m_visualizer->move2Up(true);
            break;
        case Qt::Key_Down:
            // player 2 down key
            m_visualizer->move2Down(true);
            break;
        default:
            break;
        }
        return true;
    } else if(keyEvent->type() == QEvent::KeyRelease) {
        switch (keyEvent->key()) {
        case Qt::Key_W:
            m_visualizer->move1Up(false);
            // player 1 up key
            break;
        case Qt::Key_S:
            // player 1 down key
            m_visualizer->move1Down(false);
            break;
        case Qt::Key_Up:
            // player 2 up key
            m_visualizer->move2Up(false);
            break;
        case Qt::Key_Down:
            // player 2 down key
            m_visualizer->move2Down(false);
            break;
        default:
            break;
        }
        return true;
    }

    return false;
}


ldAbstractGameVisualizer *ldPong::getGameVisualizer() const
{
    return m_visualizer.data();
}
