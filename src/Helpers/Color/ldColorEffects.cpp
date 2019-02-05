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

#include <ldCore/Helpers/Color/ldColorEffects.h>

#include <QtCore/QTime>

#include <ldCore/Helpers/Color/ldColorUtil.h>

ldAbstractColorEffect::~ldAbstractColorEffect()
{
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
    Vec2 dimOrigin = p_dim.bottom_left;
    Vec2 dimSize = p_dim.size();

    int color = ldColorUtil::colorHSV(_lastColorStep, 1.0, 1.0);
    //
    Vec2 p = ldMaths::laserToUnitedCoords(p_point);
    //
    int first_key = _lastSecond;
    if (first_key > 9) first_key = _lastSecond - 10*(int)(_lastSecond/10); // only 0 to 9
    if (first_key > 4) first_key = first_key - 5;  // only 0 to 4
    bool test = false;
    if (first_key == 0) test = (p.y < dimOrigin.y + dimSize.y*_millis/1000);
    if (first_key == 1) test = (p.y < dimOrigin.y + dimSize.y*(1.0-_millis/1000));
    if (first_key == 2) test = (p.x < dimOrigin.x + dimSize.x*_millis/1000);
    if (first_key == 3) test = (p.x < dimOrigin.x + dimSize.x*(1.0-_millis/1000));
    if (first_key == 4) test = ((p.y < dimOrigin.y + dimSize.y*_millis/1000) && (p.x < (dimOrigin.x + dimSize.x)*_millis/1000));
    if (test) color = ldColorUtil::colorHSV(ldMaths::periodIntervalKeeper(_lastColorStep+_baseColorDecay, 0, 360), 1.0, 1.0);
    return color;
}

uint32_t ldColorEffectTwo::getColor(const Vec2 &p_point, const SvgDim &p_dim)
{
    Vec2 dimOrigin = p_dim.bottom_left;
    Vec2 dimSize = p_dim.size();

    int color = ldColorUtil::colorHSV(_lastColorStep, 1.0, 1.0);
    //
    Vec2 p = ldMaths::laserToUnitedCoords(p_point);
    //
    int first_key = _lastSecond;
    bool test = false;

    if (first_key%2 == 0) test = (p.y < dimOrigin.y + dimSize.y*_millis/1000);
    else test = (p.y < dimOrigin.y + dimSize.y*(1.0-_millis/1000));

    //
    if (test) color = ldColorUtil::colorHSV(ldMaths::periodIntervalKeeper(_lastColorStep+_baseColorDecay, 0, 360), 1.0, 1.0);
    //
    float dec = dimSize.y / 10.0f;
    if (
        ((first_key%2 == 0) && fabs( (dimOrigin.y + dimSize.y*(_millis/1000)) - p.y) < dec)
        || ((first_key%2 != 0) && fabs( (dimOrigin.y + dimSize.y*(1.0-_millis/1000)) - p.y) < dec)
        ){
        color= 0xFFFFFF;
    }
    return color;
}

uint32_t ldColorEffectThree::getColor(const Vec2 &p_point, const SvgDim &p_dim)
{
    Vec2 p = ldMaths::laserToUnitedCoords(p_point);
    //
    float x = 360.f*(_millis/1000.f) + _baseColorDecay * fabsf( (p_dim.bottom_left.y + p_dim.size().y) - p.y)/p_dim.size().y;
    float colorH = ldMaths::periodIntervalKeeper(x, 0, 360);
    return ldColorUtil::colorHSV(colorH, 1.0, 1.0);
}






