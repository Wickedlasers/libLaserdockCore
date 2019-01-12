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

#ifndef LDLASERMANAGER_H
#define LDLASERMANAGER_H

#include <QQmlHelpers>

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT ldLaserController : public QObject
{
    Q_OBJECT
    QML_READONLY_PROPERTY(int, connectedDevices)
    QML_READONLY_PROPERTY(bool, isActive)

public:
    explicit ldLaserController(QObject *parent = 0);

public slots:
    void togglePlay();

private slots:
    void onHardwareDeviceCountChanged(uint count);

private:
    void refreshDeviceState();
    void refreshPlayState();
};

#endif // LDLASERMANAGER_H
