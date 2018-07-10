#ifndef LDMIDIINFO_H
#define LDMIDIINFO_H

#include <QtCore/QObject>

#ifdef Q_OS_MAC
#include <CoreMIDI/CoreMIDI.h>
#endif

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT ldMidiInfo {
public:
    static void registerMetaType();

    ldMidiInfo() = default;
    ldMidiInfo(int id, const QString &name);

    int id() const;
    QString name() const;

    bool isValid() const;

private:
    int m_id = -1;
    QString m_name;

    friend QDataStream &operator>>(QDataStream &in, ldMidiInfo &myObj);
};

// required for QVariant conversion
QDataStream &operator<<(QDataStream &out, const ldMidiInfo &myObj);
QDataStream &operator>>(QDataStream &in, ldMidiInfo &myObj);

Q_DECLARE_METATYPE(ldMidiInfo)

#endif // LDMIDIINFO_H
