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

#ifndef LDCOLORUTIL_H
#define LDCOLORUTIL_H

#include <QtCore/QtGlobal>

#include "ldCore/Shape/ldParticleGeometry.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

namespace ldColorUtil
{
    LDCORESHARED_EXPORT int lerpInt(int a, int b, float amt);
    LDCORESHARED_EXPORT float lerpF(float a, float b, float amt);
    LDCORESHARED_EXPORT uint32_t brgToRgb(int brg);
    
    LDCORESHARED_EXPORT uint32_t colorForStep(float step, float decay = 0);
    
    LDCORESHARED_EXPORT void rgb2hsv(float& r, float& g, float& b, float& h, float& s, float& v);
    LDCORESHARED_EXPORT void hsv2rgb(float& h, float& s, float& v, float& r, float& g, float& b);
    LDCORESHARED_EXPORT void hueSet(float& r, float& g, float& b, float hue);
    LDCORESHARED_EXPORT void hueShift(float& r, float& g, float& b, float hueoffset);

    LDCORESHARED_EXPORT uint32_t colorRGB(uint32_t r, uint32_t g, uint32_t b);

    /** Get RGB color from HSV */
    LDCORESHARED_EXPORT uint32_t colorHSV(float h, float s, float v);

    /** float color space conversions, domain and range are [0,1] */
    LDCORESHARED_EXPORT void colorHSVtoRGBfloat(float h, float s, float v, float& r, float& g, float& b);
    LDCORESHARED_EXPORT void colorRGBtoHSVfloat(float r, float g, float b, float& h, float& s, float& v);

    LDCORESHARED_EXPORT void HSVtoRGB(quint16 h, quint8 s, quint8 v, quint8 wheelLine, quint8 color[3]);
    LDCORESHARED_EXPORT void RGBtoHSV(quint8 r, quint8 g, quint8 b, quint16 hsv[3]);
}

#endif // LDCOLORUTIL_H

