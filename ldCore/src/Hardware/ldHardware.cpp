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

#include "ldCore/Hardware/ldHardware.h"

#include <QtDebug>

int ldHardware::REMOTE_MAX_BUFFER = 768;

ldHardware::~ldHardware()
{
}

ldHardware::Status ldHardware::status()
{
    return m_status;
}

bool ldHardware::isActive() const
{
    return m_isActive;
}

void ldHardware::setActive(bool active)
{
    m_isActive = active;
}

ldHardware::ldHardware(QObject *parent)
    : QObject(parent)
{
}

void ldHardware::setStatus(ldHardware::Status status)
{
    if(m_status == status) {
        return;
    }

    m_status = status;

    emit statusChanged(status);
}

