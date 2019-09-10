#ifndef LDMIDIINFO_H
#define LDMIDIINFO_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

class ldSoundDeviceInfo;

class LDCORESHARED_EXPORT ldMidiInfo {
public:
    static void registerMetaType();

    static ldMidiInfo fromSoundDeviceInfo(const ldSoundDeviceInfo &info);

    ldMidiInfo() = default;
    ldMidiInfo(int id, const QString &name, const QVariant &data = QVariant());

    int id() const;
    QString name() const;
    QVariant data() const;

    bool isValid() const;

    bool operator ==(const ldMidiInfo& other) const;
    bool operator !=(const ldMidiInfo& other) const;

    ldSoundDeviceInfo toSoundDeviceInfo() const;

private:
    int m_id = -1;
    QString m_name;
    QVariant m_data;

    friend QDataStream &operator>>(QDataStream &in, ldMidiInfo &myObj);
};

// required for QVariant conversion
QDataStream &operator<<(QDataStream &out, const ldMidiInfo &myObj);
QDataStream &operator>>(QDataStream &in, ldMidiInfo &myObj);

Q_DECLARE_METATYPE(ldMidiInfo)

#endif // LDMIDIINFO_H
