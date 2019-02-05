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

#ifndef LDSOUNDDEVICEINFO_H
#define LDSOUNDDEVICEINFO_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT ldSoundDeviceInfo {
public:
    enum Type {
        None = 0x00,
        QAudioInput = 0x01,
        Loopback = 0x02,
#ifdef LD_CORE_ENABLE_MIDI
        Midi = 0x04,
#endif
        Stub = 0x08
    };

    static void registerMetaType();

    ldSoundDeviceInfo();

    ldSoundDeviceInfo(Type type, const QString &name, QVariant id = QVariant(), QVariant data = QVariant());

    Type type() const;
    QString name() const;
    QVariant id() const;
    QVariant data() const;

    bool operator ==(const ldSoundDeviceInfo& other);

    bool isValid() const;

private:
    Type m_type = Type::None;
    QString m_name;
    QVariant m_id;
    QVariant m_data;

    friend QDataStream &operator>>(QDataStream &in, ldSoundDeviceInfo &myObj);
};

// required for QVariant conversion
QDataStream &operator<<(QDataStream &out, const ldSoundDeviceInfo &myObj);
QDataStream &operator>>(QDataStream &in, ldSoundDeviceInfo &myObj);

Q_DECLARE_METATYPE(ldSoundDeviceInfo)

#endif // LDSOUNDDEVICEINFO_H
