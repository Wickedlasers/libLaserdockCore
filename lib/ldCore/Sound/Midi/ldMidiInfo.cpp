#include "ldMidiInfo.h"

#include <QtCore/QtDebug>

void ldMidiInfo::registerMetaType()
{
    qRegisterMetaType<QList<ldMidiInfo>>("QList<ldMidiInfo>");
    qRegisterMetaTypeStreamOperators<ldMidiInfo>("ldMidiInfo");
}

ldMidiInfo::ldMidiInfo(int id, const QString &name)
    : m_id(id)
    , m_name(name)
{
}

int ldMidiInfo::id() const
{
    return m_id;
}

QString ldMidiInfo::name() const
{
    return m_name;
}

bool ldMidiInfo::isValid() const
{
    return !(m_id == -1 && m_name.isEmpty());
}

// -----------------------------------------------

#include <QDataStream>

QDataStream &operator<<(QDataStream &out, const ldMidiInfo &myObj) {
    out << myObj.id() << myObj.name();
    return out;
}

QDataStream &operator>>(QDataStream &in, ldMidiInfo &myObj) {
    int id;
    QString name;
    in >> id >> name;

    myObj.m_id = id;
    myObj.m_name = name;
    return in;
}

