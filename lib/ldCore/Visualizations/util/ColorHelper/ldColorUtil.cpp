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

#include "ldColorUtil.h"

#include <QtCore/QTime>

#include <ldCore/Filter/ldColorUtils.h>
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"
#include "ldCore/Visualizations/util/MathsHelper/ldGeometryUtil.h"

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
    return colorHSV(decColor1, 1.0f, 1.0f);
}

// colorEffectOneSecond
uint32_t ldColorUtil::colorEffectOneSecond(int p_baseColorDecay, int p_lastStep, int p_lastSecond, float p_millisec,
                                           const Vec2& p_origin, const Vec2& p_size, const Vec2& p_point)
{
    int color = colorHSV(p_lastStep, 1.0, 1.0);
    //
    Vec2 p = ldMaths::laserToUnitedCoords(p_point);
    //
    int first_key = p_lastSecond;
    if (first_key > 9) first_key = p_lastSecond - 10*(int)(p_lastSecond/10); // only 0 to 9
    if (first_key > 4) first_key = first_key - 5;  // only 0 to 4
    bool test = false;
    if (first_key == 0) test = (p.y < p_origin.y + p_size.y*p_millisec/1000);
    if (first_key == 1) test = (p.y < p_origin.y + p_size.y*(1.0-p_millisec/1000));
    if (first_key == 2) test = (p.x < p_origin.x + p_size.x*p_millisec/1000);
    if (first_key == 3) test = (p.x < p_origin.x + p_size.x*(1.0-p_millisec/1000));
    if (first_key == 4) test = ((p.y < p_origin.y + p_size.y*p_millisec/1000) && (p.x < (p_origin.x + p_size.x)*p_millisec/1000));
    if (test) color = colorHSV(ldMaths::periodIntervalKeeper(p_lastStep+p_baseColorDecay, 0, 360), 1.0, 1.0);
    return color;
}

// colorEffectTwoSecond
uint32_t ldColorUtil::colorEffectTwoSecond(int p_baseColorDecay, int p_lastStep, int p_lastSecond, float p_millisec,
                                           const Vec2& p_origin, const Vec2& p_size, const Vec2& p_point)
{
    int color = colorHSV(p_lastStep, 1.0, 1.0);
    //
    Vec2 p = ldMaths::laserToUnitedCoords(p_point);
    //
    int first_key = p_lastSecond;
    bool test = false;

    if (first_key%2 == 0) test = (p.y < p_origin.y + p_size.y*p_millisec/1000);
    else test = (p.y < p_origin.y + p_size.y*(1.0-p_millisec/1000));
    
    //
    if (test) color = colorHSV(ldMaths::periodIntervalKeeper(p_lastStep+p_baseColorDecay, 0, 360), 1.0, 1.0);
    //
    float dec = p_size.y / 10.0f;
    if (
        ((first_key%2 == 0) && fabs( (p_origin.y + p_size.y*(p_millisec/1000)) - p.y) < dec)
        || ((first_key%2 != 0) && fabs( (p_origin.y + p_size.y*(1.0-p_millisec/1000)) - p.y) < dec)
        ){
        color= 0xFFFFFF;
    }
    return color;
}

// colorEffectThree
uint32_t ldColorUtil::colorEffectThree(int p_baseColorDecay, float p_millisec, const Vec2& p_origin, const Vec2& p_size, const Vec2& p_point)
{
    Vec2 p = ldMaths::laserToUnitedCoords(p_point);
    //
    float x = 360.f*(p_millisec/1000.f) + p_baseColorDecay * fabsf( (p_origin.y + p_size.y) - p.y)/p_size.y;
    float colorH = ldMaths::periodIntervalKeeper(x, 0, 360);
    int color = colorHSV(colorH, 1.0, 1.0);
    return color;
}


void ldAbstractColorEffect::setBaseColorDecay(int baseColorDecay)
{
    _baseColorDecay = baseColorDecay;
}

void ldAbstractColorEffect::updateColor()
{
    QTime time = QTime::currentTime();
    _millis = time.msec();
}

void ldAbstractStepColorEffect::updateColor()
{
    ldAbstractColorEffect::updateColor();

    QTime time = QTime::currentTime();

    if (time.second() != _lastSecond) {
        _lastSecond = time.second();
        _lastColorStep += _baseColorDecay;
        _lastColorStep = ldMaths::periodIntervalKeeper(_lastColorStep, 0, 360);
    }
}

uint32_t ldColorEffectOne::getColor(const Vec2 &p_point, const SvgDim &p_dim)
{
    return ldColorUtil::colorEffectOneSecond(_baseColorDecay, (int) _lastColorStep, _lastSecond, _millis, p_dim.bottom_left, p_dim.size(), p_point);
}

uint32_t ldColorEffectTwo::getColor(const Vec2 &p_point, const SvgDim &p_dim)
{
    return ldColorUtil::colorEffectTwoSecond(_baseColorDecay, _lastColorStep, _lastSecond, _millis, p_dim.bottom_left, p_dim.size(), p_point);
}

uint32_t ldColorEffectThree::getColor(const Vec2 &p_point, const SvgDim &p_dim)
{
    return ldColorUtil::colorEffectThree(_baseColorDecay, _millis, p_dim.bottom_left, p_dim.size(), p_point);
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
    uint32_t c = colorHSV(h * 359, s, v);
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

} // namespace



