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

#include "ldCore/Utilities/ldBasicDataStructures.h"

#include <cstring>

void Vertex::clear()
{
    memset(this, 0, sizeof(Vertex));
}

bool Vertex::isValid() const
{
    return !(position[0] == 0 && position[1] == 0 && position[2] == 0
            && color[0] == 0 && color[1] == 0 && color[2] == 0 && color[3] == 0);
}

bool Vertex::isBlank() const
{
    return color[0] == 0 && color[1] == 0 && color[2] == 0;
}

CompressedSample::CompressedSample()
{
}

CompressedSample::CompressedSample(const Vertex &v)
{
    uint8_t red     = GetUInt8(v.color[0]);
    uint8_t green   = GetUInt8(v.color[1]);
    rg = (0x00FF & red) | ((0x00FF & green) << 8);
    b = (uint16_t) GetUInt8(v.color[2]);
    x = 4095 - GetUInt16(v.position[0]);
    y = GetUInt16(v.position[1]);
}
