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

#ifndef LDABSTRACTDATAWORKER_H
#define LDABSTRACTDATAWORKER_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

/** Base data worker interface */
class LDCORESHARED_EXPORT ldAbstractDataWorker : public QObject
{
    Q_OBJECT
public:
    virtual bool isActiveTransfer() const = 0;
    virtual bool hasActiveDevices() const = 0;

public slots:
    virtual void setActive(bool active) = 0;
    virtual void setActiveTransfer(bool active) = 0;

signals:
    void isActiveTransferChanged(bool active);

protected:
    explicit ldAbstractDataWorker(QObject *parent = nullptr);

};

#endif // LDABSTRACTDATAWORKER_H
