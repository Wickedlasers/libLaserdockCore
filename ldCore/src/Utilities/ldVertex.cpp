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

#include "ldCore/Utilities/ldVertex.h"

#include <QtCore/QtDebug>

#include <ldCore/Helpers/Maths/ldMaths.h>

void ldVertex::registerMetaTypes()
{
    QMetaType::registerDebugStreamOperator<ldVertex>();
}

float &ldVertex::x()
{
    return position[0];
}

float ldVertex::x() const
{
    return position[0];
}

float &ldVertex::y()
{
    return position[1];
}

float ldVertex::y() const
{
    return position[1];
}

float &ldVertex::r()
{
    return color[0];
}

float ldVertex::r() const
{
    return color[0];
}

float &ldVertex::g()
{
    return color[1];
}

float ldVertex::g() const
{
    return color[1];
}

float &ldVertex::b()
{
    return color[2];
}

float ldVertex::b() const
{
    return color[2];
}

bool ldVertex::isValid() const
{
    return !(position[0] == 0 && position[1] == 0
             && color[0] == 0 && color[1] == 0 && color[2] == 0);
}

bool ldVertex::isBlank() const
{
    return color[0] == 0 && color[1] == 0 && color[2] == 0;
}

void ldVertex::clear()
{
    memset(this, 0, sizeof(ldVertex));
}

void ldVertex::clearColor()
{
    for(int i = 0; i < COLOR_COUNT; i++)
        color[i] = 0;
}


void ldVertex::normalize(const ldVertex &source, float intensity)
{
    for(int i = 0; i < POS_COUNT; i++)
        position[i] = ldMaths::normalize(position[i], source.position[i], intensity);

    for(int i = 0; i < COLOR_COUNT; i++)
        color[i] = ldMaths::normalize(color[i], source.color[i], intensity);
}


QDebug operator <<(QDebug debug, const ldVertex &v)
{
    QDebugStateSaver saver(debug);

    debug.nospace() << "Vertex(" << v.x() << ", " << v.y() << ", " << "; " << v.r() << ", " << v.g() << ", " << v.b() << ", " << ")";

    return debug;
}
