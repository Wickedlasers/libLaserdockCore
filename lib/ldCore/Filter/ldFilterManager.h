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

#ifndef LDFILTERMANAGER_H
#define LDFILTERMANAGER_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

#include "ldFilterBasicGlobal.h"
#include "ldFilterBasicData.h"

class ldFilter;

class LDCORESHARED_EXPORT ldFilterManager : public QObject
{
    Q_OBJECT

public:
    explicit ldFilterManager(QObject *parent = 0);    

    ldFilterBasicGlobal m_basicGlobalFilter;
    ldFilterBasicData m_dataFilter;

    ldFilter* m_globalFilter = nullptr;
};

#endif // LDFILTERMANAGER_H
