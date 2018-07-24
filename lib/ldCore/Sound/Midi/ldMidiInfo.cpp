#include "ldMidiInfo.h"

#include <QtCore/QtDebug>

void ldMidiInfo::registerMetaType()
{
    qRegisterMetaType<QList<ldMidiInfo>>("QList<ldMidiInfo>");
    qRegisterMetaTypeStreamOperators<ldMidiInfo>("ldMidiInfo");
}

ldMidiInfo::ldMidiInfo(int id, const QString &name, const QVariant &data)
    : m_id(id)
    , m_name(name)
    , m_data(data)
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

QVariant ldMidiInfo::data() const
{
    return m_data;
}

bool ldMidiInfo::isValid() const
{
    return !(m_id == -1 && m_name.isEmpty());
}

// -----------------------------------------------

#include <QDataStream>

QDataStream &operator<<(QDataStream &out, const ldMidiInfo &myObj) {
    out << myObj.id() << myObj.name() << myObj.data();
    return out;
}

QDataStream &operator>>(QDataStream &in, ldMidiInfo &myObj) {
    int id;
    QString name;
    QVariant data;
    in >> id >> name >> data;

    myObj.m_id = id;
    myObj.m_name = name;
    myObj.m_data = data;
    return in;
}

