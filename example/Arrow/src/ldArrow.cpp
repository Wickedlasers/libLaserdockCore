#include "ldArrow.h"

#include <QtGui/QtEvents>

#include "vis/ldArrowVisualizer.h"

void ldArrow::initResources()
{
    Q_INIT_RESOURCE(arrow);
}

ldArrow::ldArrow(QObject *parent)
    : ldAbstractGame("arrow", tr("Arrow"), parent)
    , m_visualizer(new ldArrowVisualizer) {

    m_keyDescriptions << "Left" << "Move Left";
    m_keyDescriptions << "Right" << "Move Right";

    QMap<ldGamepad::Button, Qt::Key> keyMap;
    keyMap[ldGamepad::Button::Left] = Qt::Key_Left;
    keyMap[ldGamepad::Button::Right] = Qt::Key_Right;

    get_gamepadCtrl()->init(keyMap, m_keyDescriptions);
}

ldArrow::~ldArrow() {
}

bool ldArrow::handleKeyEvent(QKeyEvent *keyEvent) {
    if(keyEvent->type() == QEvent::KeyPress) {
        if (keyEvent->key() == Qt::Key_Left){
            m_visualizer->keyLeft(true);
        } else if (keyEvent->key() == Qt::Key_Right) {
            m_visualizer->keyRight(true);
        }
        return true;
    } else if(keyEvent->type() == QEvent::KeyRelease) {
        if (keyEvent->key() == Qt::Key_Left){
            m_visualizer->keyLeft(false);
        } else if (keyEvent->key() == Qt::Key_Right) {
            m_visualizer->keyRight(false);
        }
        return true;
    }
    return false;
}

ldAbstractGameVisualizer *ldArrow::getGameVisualizer() const {
    return m_visualizer.data();
}
