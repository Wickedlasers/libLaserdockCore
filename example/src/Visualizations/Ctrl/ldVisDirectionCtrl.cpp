#include "ldVisDirectionCtrl.h"

#include <QtQml/QtQml>

void ldVisDirectionCtrl::registerMetaTypes()
{
    qmlRegisterUncreatableType<ldVisDirectionCtrl>("WickedLasers", 1, 0, "LdVisDirection", "LdVisDirection enum can't be created");
}

ldVisDirectionCtrl::ldVisDirectionCtrl(QObject *parent)
    : QObject(parent)
    , m_isActive(false)
    , m_mutex(QMutex::Recursive)
{
}

void ldVisDirectionCtrl::next()
{
    QMutexLocker lock(&m_mutex);

    int nextDirection = m_direction + 1;
    if(nextDirection > LeftRight) {
        nextDirection = 0;
    }
    update_direction(nextDirection);
}


void ldVisDirectionCtrl::reset()
{
    QMutexLocker lock(&m_mutex);

    update_direction(Right);
}

int ldVisDirectionCtrl::get_direction() const
{
    QMutexLocker lock(&m_mutex);

    return m_direction;
}

bool ldVisDirectionCtrl::update_direction(int direction)
{
    bool ret = false;
    if ((ret = (m_direction != direction))) {
        m_direction = direction;
        emit directionChanged(m_direction);
    }
    return ret;
}


