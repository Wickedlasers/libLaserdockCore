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
//  ldColorUtilMEO.cpp
//  ldCore
//
//  Created by MEO
//  Copyright © 2016 Wicked Lasers. All rights reserved.
//
#include "ldCore/Helpers/Color/ldColorUtilMEO.h"

/* MEO */

// Hue is 0 to 1536 when wheelLine is 0 (otherwise 1024)
// Sat is 0 to 255
// Level is 0 to 255
// wheelLine of 0 is full color wheel, 1 is RG only, 2 GB, 3 BR.
// color is passed back - array of R0-255, G0-255, B0-255
void ldColorUtilMEO::HSVtoRGB(quint16 h, quint8 s, quint8 v, quint8 wheelLine, quint8 color[3]) {
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
void ldColorUtilMEO::RGBtoHSV(quint8 r, quint8 g, quint8 b, quint16 hsv[3]) {
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

// Convert separate R,G,B into combined 24-bit RGB color:
quint32 ldColorUtilMEO::convertRGBtoColor(quint8 r, quint8 g, quint8 b) {
    return b + (g * 256) + (r * 256 * 256);
}
