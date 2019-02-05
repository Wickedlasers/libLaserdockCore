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

//
//  ldColorUtil.cpp
//
//  Created by Eric Brugère on 1/19/15. Ammended by MEO on 18th April 2015
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Color/ldColorUtil.h"

#include <QtCore/QTime>

#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

/* Eric Brugère */

// lerpInt
int ldColorUtil::lerpInt(int a, int b, float amt)
{
    int c[3];
    int a_[3];
    int b_[3];
    //
    a_[0] = a >> 16 & 0xFF;
    a_[1] = a >> 8 & 0xFF;
    a_[2] = a & 0xFF;
    b_[0] = b >> 16 & 0xFF;
    b_[1] = b >> 8 & 0xFF;
    b_[2] = b & 0xFF;
    //
    amt=clampf(amt, 0.0, 1.0);
    for (int i=0; i<3; i++)
    {
        c[i] = (int)(a_[i] + (b_[i] - a_[i]) * amt);
    }
    return ((c[0] << 16)+(c[1] << 8)+(c[2]));
}


float ldColorUtil::lerpF(float a, float b, float amt)
{
    return (a + amt*(b - a));
}

// brgToRgb
uint32_t ldColorUtil::brgToRgb(int brg) {
    int b = brg >> 16 & 0xFF;
    int g = brg >> 8 & 0xFF;
    int r = brg & 0xFF;
    // change pure black in white
    if (b == 0 && r==0 && g==0) {
        return 0xFFFFFF;
    }
    //
    r =  (r << 16);
    g =  (g << 8);
    return (r+g+b);
}

// colorForStep
uint32_t ldColorUtil::colorForStep(float step, float decay)
{
    float decColor1 = step == 0 ? 0 : (1.0f - step)*360.0f;
    decColor1 = ldMaths::periodIntervalKeeper(decColor1 + decay*360.0f, 0.0f, 360.0f);
    return ldColorUtil::colorHSV(decColor1, 1.0f, 1.0f);
}

namespace ldColorUtil {

void rgb2hsv(float& r, float& g, float& b, float& h, float& s, float& v) {
    clampfp(r, 0, 1);
    clampfp(g, 0, 1);
    clampfp(b, 0, 1);

    float maxc = 0;
    maxc = MAX(maxc, r);
    maxc = MAX(maxc, g);
    maxc = MAX(maxc, b);
    float minc = 1;
    minc = MIN(minc, r);
    minc = MIN(minc, g);
    minc = MIN(minc, b);
    float delta = maxc - minc;

    float hue6 = 0;
    if (delta > 0) {
        if (maxc == r) {
            hue6 =  (g - b) / delta + 0;
        } else if (maxc == g) {
            hue6 =  (b - r) / delta + 2;
        } else {
            hue6 =  (r - g) / delta + 4;
        }
    }
    h = hue6/6;
    h -= (int)h;

    float sat = 0;
    if (maxc > 0) sat = delta / maxc;
    s = sat;

    v = maxc;

    clampfp(h, 0, 1);
    clampfp(s, 0, 1);
    clampfp(v, 0, 1);
}

void hsv2rgb(float& h, float& s, float& v, float& r, float& g, float& b) {
    clampfp(h, 0, 1);
    clampfp(s, 0, 1);
    clampfp(v, 0, 1);
    uint32_t c = ldColorUtil::colorHSV(h * 359, s, v);
    r = ((c >> 16) & 0xff) / 255.0f;
    g = ((c >> 8) & 0xff) / 255.0f;
    b = ((c >> 0) & 0xff) / 255.0f;
    clampfp(r, 0, 1);
    clampfp(g, 0, 1);
    clampfp(b, 0, 1);
}

void hueSet(float& r, float& g, float& b, float hue) {
    float h, s, v;
    rgb2hsv(r, g, b, h, s, v);
    h = hue;
    h -= (int)floorf(h);
    hsv2rgb(h, s, v, r, g, b);
}

void hueShift(float& r, float& g, float& b, float hueoffset) {
    float h, s, v;
    rgb2hsv(r, g, b, h, s, v);
    h += hueoffset;
    h -= (int)floorf(h);
    hsv2rgb(h, s, v, r, g, b);
}


uint32_t colorRGB(uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t result = (r << 16) | (g << 8) | (b);

    return result;
}

uint32_t colorHSV(float h, float s, float v)
{
    int i;
    float f, p, q, t;

    uint32_t r, g, b;

    if (s == 0)
    {
        // achromatic (grey)
        r = g = b = 255 * v;
    }

    h /= 60;                    // sector 0 to 5
    i = floor(h);
    f = h - i;                  // factorial part of h
    p = v * (1 - s);
    q = v * (1 - s * f);
    t = v * (1 - s * (1 - f));

    switch (i)
    {
        case 0:
            r = v * 255;
            g = t * 255;
            b = p * 255;
            break;
        case 1:
            r = q * 255;
            g = v * 255;
            b = p * 255;
            break;
        case 2:
            r = p * 255;
            g = v * 255;
            b = t * 255;
            break;
        case 3:
            r = p * 255;
            g = q * 255;
            b = v * 255;
            break;
        case 4:
            r = t * 255;
            g = p * 255;
            b = v * 255;
            break;
        default: // case 5
            r = v * 255;
            g = p * 255;
            b = q * 255;
            break;
    }

    uint32_t result = (r << 16) | (g << 8) | (b);

    return result;
}



// internal
static float clamp(float& i) {if (i < 0) return i=0; if (i > 1) return i=1; return i; }
#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN
#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

void colorRGBtoHSVfloat(float r, float g, float b, float& h, float& s, float& v) {
    clamp(r);
    clamp(g);
    clamp(b);

    // min max delta
    float maxc = 0;
    maxc = MAX(maxc, r);
    maxc = MAX(maxc, g);
    maxc = MAX(maxc, b);
    float minc = 1;
    minc = MIN(minc, r);
    minc = MIN(minc, g);
    minc = MIN(minc, b);
    float delta = maxc - minc;

    // hue calc
    float hue6 = 0;
    if (delta > 0) {
        if (maxc == r) {
            hue6 =  (g - b) / delta + 0;
        } else if (maxc == g) {
            hue6 =  (b - r) / delta + 2;
        } else {
            hue6 =  (r - g) / delta + 4;
        }
    }

    // hsv
    h = hue6/6;  h -= (int)h;
    if (maxc > 0) s = delta / maxc; else s = 0;
    v = maxc;

    clamp(h);
    clamp(s);
    clamp(v);

}

void colorHSVtoRGBfloat(float h, float s, float v, float& r, float& g, float& b) {
    clamp(h);
    clamp(s);
    clamp(v);
    uint32_t c = colorHSV(h * 359, s, v);
    r = ((c >> 16) & 0xff) / 255.0f;
    g = ((c >> 8) & 0xff) / 255.0f;
    b = ((c >> 0) & 0xff) / 255.0f;
    clamp(r);
    clamp(g);
    clamp(b);
}


// Hue is 0 to 1536 when wheelLine is 0 (otherwise 1024)
// Sat is 0 to 255
// Level is 0 to 255
// wheelLine of 0 is full color wheel, 1 is RG only, 2 GB, 3 BR.
// color is passed back - array of R0-255, G0-255, B0-255
void HSVtoRGB(quint16 h, quint8 s, quint8 v, quint8 wheelLine, quint8 color[3]) {
    int r, g, b, lo;
    r = g = b = lo = 0;
    switch (wheelLine) {
        case 0: // Full RGB Wheel (based on pburgess original function)
            h %= 1536;
            // h - unsigned
//            if (h < 0) h += 1536; //     0 to +1535
            lo = h & 255; // Low byte  = primary/secondary color mix
            switch (h >> 8) { // High byte = sextant of colorwheel
                case 0:
                    r = 255;
                    g = lo;
                    b = 0;
                    break; // R to Y
                case 1:
                    r = 255 - lo;
                    g = 255;
                    b = 0;
                    break; // Y to G
                case 2:
                    r = 0;
                    g = 255;
                    b = lo;
                    break; // G to C
                case 3:
                    r = 0;
                    g = 255 - lo;
                    b = 255;
                    break; // C to B
                case 4:
                    r = lo;
                    g = 0;
                    b = 255;
                    break; // B to M
                default:
                    r = 255;
                    g = 0;
                    b = 255 - lo;
                    break; // M to R
            }
            break;
        case 1: //RG Line only
            h %= 1024;
//            if (h < 0) h += 1024;
            lo = h & 255; // Low byte  = primary/secondary color mix
            switch (h >> 8) { // High byte = sextant of colorwheel
                case 0:
                    r = 255;
                    g = lo;
                    b = 0;
                    break; // R to Y
                case 1:
                    r = 255 - lo;
                    g = 255;
                    b = 0;
                    break; // Y to G
                case 2:
                    r = lo;
                    g = 255;
                    b = 0;
                    break; // G to Y
                default:
                    r = 255;
                    g = 255 - lo;
                    b = 0;
                    break; // Y to R
            }
            break;
        case 2: //GB Line only
            h %= 1024;
//            if (h < 0) h += 1024;
            lo = h & 255; // Low byte  = primary/secondary color mix
            switch (h >> 8) { // High byte = sextant of colorwheel
                case 0:
                    r = 0;
                    g = 255;
                    b = lo;
                    break; // G to C
                case 1:
                    r = 0;
                    g = 255 - lo;
                    b = 255;
                    break; // C to B
                case 2:
                    r = 0;
                    g = lo;
                    b = 255;
                    break; // B to C
                default:
                    r = 0;
                    g = 255;
                    b = 255 - lo;
                    break; // C to G
            }
            break;
        case 3: //BR Line only
            h %= 1024;
//            if (h < 0) h += 1024;
            lo = h & 255; // Low byte  = primary/secondary color mix
            switch (h >> 8) { // High byte = sextant of colorwheel
                case 0:
                    r = lo;
                    g = 0;
                    b = 255;
                    break; // B to M
                case 1:
                    r = 255;
                    g = 0;
                    b = 255 - lo;
                    break; // M to R
                case 2:
                    r = 255;
                    g = 0;
                    b = lo;
                    break; // R to M
                default:
                    r = 255 - lo;
                    g = 0;
                    b = 255;
                    break; // M to B
            }
            break;
    }

    r = 255 - (((255 - r) * (s + 1)) >> 8);
    g = 255 - (((255 - g) * (s + 1)) >> 8);
    b = 255 - (((255 - b) * (s + 1)) >> 8);

    color[0] = (quint8) (r * (v + 1));
    color[1] = (quint8) (g * (v + 1));
    color[2] = (quint8) (b * (v + 1));
}

// compliment to HSVtoRGB
void RGBtoHSV(quint8 r, quint8 g, quint8 b, quint16 hsv[3]) {
    float tempR, tempG, tempB;

    tempR = (float)r / 255.0;
    tempG = (float)g / 255.0;
    tempB = (float)b / 255.0;

    float minRGB = qMin(tempR, qMin(tempG, tempB));
    float maxRGB = qMax(tempR, qMax(tempG, tempB));

    // Black-gray-white
    if (minRGB == maxRGB) {
        hsv[0] = 0; // irrelevant, as saturation is nothing
        hsv[1] = 0;
        hsv[2] = (quint16) (255.0 * maxRGB);
    } else {
        // Colors other than black-gray-white:
        float d = (tempR == minRGB) ? tempG - tempB : ((tempB == minRGB) ? tempR - tempG : tempB - tempR);
        float h = (tempR == minRGB) ? 3.0 : ((tempB == minRGB) ? 1.0 : 5.0);
        hsv[0] = (quint16) (255.0 * (h - d / (maxRGB - minRGB)));
        hsv[1] = (quint16) (255.0 * (maxRGB - minRGB) / maxRGB);
        hsv[2] = (quint16) (255.0 * maxRGB);
    }
}

} // namespace



