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

#include "ldCore/Sound/ldSoundDeviceInfo.h"

#include <ldCore/Helpers/ldEnumHelper.h>

void ldSoundDeviceInfo::registerMetaType()
{
    qRegisterMetaType<QList<ldSoundDeviceInfo>>("QList<ldSoundDeviceInfo>");
    qRegisterMetaTypeStreamOperators<ldSoundDeviceInfo>("ldSoundDeviceInfo");
}

ldSoundDeviceInfo::ldSoundDeviceInfo()
{
}

ldSoundDeviceInfo::ldSoundDeviceInfo(Type type, const QString &name, QVariant id, QVariant data)
    : m_type(type)
    , m_name(name)
    , m_id(id)
    , m_data(data)
{
}

ldSoundDeviceInfo::Type ldSoundDeviceInfo::type() const
{
    return m_type;
}

QString ldSoundDeviceInfo::name() const
{
    return m_name;
}

QVariant ldSoundDeviceInfo::id() const
{
    return m_id;
}

QVariant ldSoundDeviceInfo::data() const
{
    return m_data;
}

bool ldSoundDeviceInfo::operator ==(const ldSoundDeviceInfo &other)
{
    return m_type == other.m_type
            &&  m_name == other.m_name
            &&  m_id == other.m_id
            &&  m_data == other.m_data
            ;
}

bool ldSoundDeviceInfo::isValid() const
{
    return m_type != Type::None && !m_name.isEmpty();
}


// -----------------------------------------------

#include <QDataStream>

QDataStream &operator<<(QDataStream &out, const ldSoundDeviceInfo &myObj) {
    int typeInt = ldEnumHelper::as_integer(myObj.type());
    out << typeInt << myObj.name() << myObj.id() << myObj.data();
    return out;
}

QDataStream &operator>>(QDataStream &in, ldSoundDeviceInfo &myObj) {
    int type;
    QString name;
    QVariant id;
    QVariant data;
    in >> type >> name >> id >> data;

    myObj.m_name = name;
    myObj.m_type = ldSoundDeviceInfo::Type(type);
    myObj.m_id = id;
    myObj.m_id = data;
    return in;
}

