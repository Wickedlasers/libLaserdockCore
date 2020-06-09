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

#ifndef LDCOMPRESSEDSAMPLE_H
#define LDCOMPRESSEDSAMPLE_H

#include <ldCore/ldCore_global.h>

struct ldVertex;

/** Samples that are used by laser hardware */
struct LDCORESHARED_EXPORT ldCompressedSample
{
    static uint16_t flipCoord(uint16_t value);

    ldCompressedSample();
    ldCompressedSample(const ldVertex &v);

    uint16_t rg = 0;
    uint16_t b = 0;
    uint16_t x = 0;
    uint16_t y = 0;

private:
    static const uint16_t MAX_COORD = 4095;
    uint16_t GetUInt16(float f) const;
    uint8_t GetUInt8(float f) const;
};

inline uint16_t ldCompressedSample::flipCoord(uint16_t value)
{
    return MAX_COORD - value;
}

inline uint16_t ldCompressedSample::GetUInt16(float f) const
{
    if (f > 1.0f) f = 1.0f;
    if (f < -1.0f) f = -1.0f;
    return static_cast<uint16_t>((f + 1.0f) / 2.f * MAX_COORD);
}

inline uint8_t ldCompressedSample::GetUInt8(float f) const
{
    if (f < 0) f = 0;
    if (f > 1) f = 1;
    return static_cast<uint8_t>(f * 255);
}

#endif // LDCOMPRESSEDSAMPLE_H
