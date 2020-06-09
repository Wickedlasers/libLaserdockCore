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

#include "ldCore/Utilities/ldCompressedSample.h"

#include <ldCore/Utilities/ldVertex.h>

ldCompressedSample::ldCompressedSample()
{
}

ldCompressedSample::ldCompressedSample(const ldVertex &v)
{
    uint8_t red     = GetUInt8(v.r());
    uint8_t green   = GetUInt8(v.g());
    rg = (0x00FF & red) | ((0x00FF & green) << 8);
    b = (uint16_t) GetUInt8(v.b());
    x = MAX_COORD - GetUInt16(v.x());
    y = GetUInt16(v.y());
}
