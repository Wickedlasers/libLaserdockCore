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

#ifndef LDCOLORUTILS
#define LDCOLORUTILS

#include <QtCore/qglobal.h>

#include "ldCore/ldCore_global.h"

LDCORESHARED_EXPORT uint32_t colorRGB(uint32_t r, uint32_t g, uint32_t b);
LDCORESHARED_EXPORT uint32_t colorHSV(float h, float s, float v);

// float color space conversions, domain and range are [0,1]
LDCORESHARED_EXPORT void colorHSVtoRGBfloat(float h, float s, float v, float& r, float& g, float& b);
LDCORESHARED_EXPORT void colorRGBtoHSVfloat(float r, float g, float b, float& h, float& s, float& v);


#endif // LDFILTER

