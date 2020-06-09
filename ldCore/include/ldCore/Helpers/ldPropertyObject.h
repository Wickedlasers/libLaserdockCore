#ifndef LDPROPERTYOBJECT_H
#define LDPROPERTYOBJECT_H

#include <QtCore/QVariant>

#include <QQmlHelpers>

#include <ldCore/ldCore_global.h>

/** QObject-wrapper that provide slots to control property values by name */
class LDCORESHARED_EXPORT ldPropertyObject : public QObject
{
    Q_OBJECT

public:
    ldPropertyObject(QObject *parent = nullptr);

public slots:
    bool setProperty(const QByteArray &name, const QVariant &value);
    QVariant property(const QByteArray &name) const;

    bool isRealProperty(const QByteArray &name) const;
};

#endif // LDPROPERTYOBJECT_H
