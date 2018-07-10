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

#include "ldColorUtils.h"

#include <cmath>

/**
 @fn uint32_t colorRGB(uint32_t r, uint32_t g, uint32_t b)
 @brief get RGB color from r, g and b values
 */
uint32_t colorRGB(uint32_t r, uint32_t g, uint32_t b)
{
    uint32_t result = (r << 16) | (g << 8) | (b);

    return result;
}

/**
 @fn uint32_t colorHSV(float h, float s, float v)
 @brief get RGB color from HSV
 */
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
