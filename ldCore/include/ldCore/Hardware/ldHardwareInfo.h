/**
    libLaserdockCore
    Copyright(c) 2021 Wicked Lasers

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

#ifndef LDHARDWAREINFO_H
#define LDHARDWAREINFO_H

#include <QQmlHelpers>
#include <QtCore/QObject>
#include <ldCore/ldCore_global.h>
#include <ldCore/Helpers/ldPropertyObject.h>

class LDCORESHARED_EXPORT ldHardwareInfo : public ldPropertyObject
{
    Q_OBJECT

    QML_READONLY_PROPERTY(bool, hasValidInfo)
    QML_READONLY_PROPERTY(int, fwMajor)
    QML_READONLY_PROPERTY(int, fwMinor)
    QML_READONLY_PROPERTY(int, dacRate)
    QML_READONLY_PROPERTY(int, maxDacRate)
    QML_READONLY_PROPERTY(int, batteryPercent)
    QML_READONLY_PROPERTY(int, modelNumber)
    QML_READONLY_PROPERTY(QString, modelName)
    QML_READONLY_PROPERTY(int, temperatureDegC)
    QML_READONLY_PROPERTY(int, connectionType)
    QML_READONLY_PROPERTY(int, bufferSize)
    QML_READONLY_PROPERTY(int, bufferFree)
    QML_READONLY_PROPERTY(int, overTemperature)
    QML_READONLY_PROPERTY(int, temperatureWarning)
    QML_READONLY_PROPERTY(int, interlockEnabled)
    QML_READONLY_PROPERTY(int, packetErrors)
    QML_READONLY_PROPERTY(QString, address)

public:
    ldHardwareInfo(QObject *parent = nullptr);
    ~ldHardwareInfo();
};

#endif // LDHARDWAREINFO_H
