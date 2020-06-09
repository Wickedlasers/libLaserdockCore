#include "ldCore/Helpers/ldPropertyObject.h"

#include <QtCore/QtDebug>
#include <QtCore/QMetaProperty>

ldPropertyObject::ldPropertyObject(QObject *parent)
    : QObject(parent)
{
}

bool ldPropertyObject::setProperty(const QByteArray &name, const QVariant &value)
{
    // check if property exists
    int propertyIndex = metaObject()->indexOfProperty(name);
    if(propertyIndex == -1) {
        qWarning() << __FUNCTION__ << "Invalid property!" << name << this;
        return false;
    }

    // check if property is writable
    bool isWritable = metaObject()->property(propertyIndex).isWritable();
    if(!isWritable) {
        qWarning() << __FUNCTION__ << "Invalid property!" << name << this;
        return false;
    }

    return QObject::setProperty(name.constData(), value);
}

QVariant ldPropertyObject::property(const QByteArray &name) const
{
    if(!QObject::property(name.constData()).isValid()) {
        qWarning() << __FUNCTION__ << "Invalid property!" << name << this;
    }

    return QObject::property(name.constData());
}

bool ldPropertyObject::isRealProperty(const QByteArray &name) const
{
    int propertyIndex = metaObject()->indexOfProperty(name);
    if(propertyIndex == -1)
        return false;

    // it's QMetaType::Type under the hood
    QMetaType::Type type = static_cast<QMetaType::Type> (metaObject()->property(propertyIndex).type());

    return type == QMetaType::Double || type == QMetaType::Float;
}
