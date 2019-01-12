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

#ifndef LDHARDWARE_H
#define LDHARDWARE_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>

#include "ldCore/ldCore_global.h"
#include "ldCore/Utilities/ldBasicDataStructures.h"

class LDCORESHARED_EXPORT ldHardware : public QObject
{
    Q_OBJECT

public:
    static int REMOTE_MAX_BUFFER;

    enum class Status { UNKNOWN, INITIALIZED };
    
    ~ldHardware();

    Status status();

    bool isActive() const;
    void setActive(bool active);

signals:
    void statusChanged(Status status);

protected:
    explicit ldHardware(QObject *parent = 0);

    void setStatus(Status status);

    bool m_isActive = false;

private:
    Status m_status = Status::UNKNOWN;
};

Q_DECLARE_METATYPE(ldHardware *)

#endif // LDHARDWARE_H
