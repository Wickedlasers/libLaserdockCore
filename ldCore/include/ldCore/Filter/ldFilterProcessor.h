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

#ifndef LDPROCESSORFILTER_H
#define LDPROCESSORFILTER_H


#include <ldCore/ldCore_global.h>

#include <ldCore/Helpers/ldPropertyObject.h>

class ldFilter;
struct ldVertex;

class LDCORESHARED_EXPORT ldFilterProcessor : public ldPropertyObject
{
    Q_OBJECT
public:
    ldFilterProcessor(QObject *parent = nullptr);
    virtual void processFilter(ldFilter *filter, ldVertex &input);
};

#endif // LDPROCESSORFILTER_H

