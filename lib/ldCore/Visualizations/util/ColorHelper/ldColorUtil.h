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
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"

struct Point3dColor {
    float x;
    float y;
    float z;
    int c;
};

namespace ldColorUtil
{
    LDCORESHARED_EXPORT int lerpInt(int a, int b, float amt);
    LDCORESHARED_EXPORT float lerpF(float a, float b, float amt);
    LDCORESHARED_EXPORT uint32_t brgToRgb(int brg);
    
    LDCORESHARED_EXPORT uint32_t colorForStep(float step, float decay = 0);
    
    LDCORESHARED_EXPORT uint32_t colorEffectOneSecond(int p_baseColorDecay, int p_lastStep, int p_lastSecond, float p_millisec, const Vec2& p_origin, const Vec2& p_size, const Vec2& p_point);
    LDCORESHARED_EXPORT uint32_t colorEffectTwoSecond(int p_baseColorDecay, int p_lastStep, int p_lastSecond, float p_millisec, const Vec2& p_origin, const Vec2& p_size, const Vec2& p_point);
    LDCORESHARED_EXPORT uint32_t colorEffectThree(int p_baseColorDecay, float p_millisec, const Vec2& p_origin, const Vec2& p_size, const Vec2& p_point);
    

    // hsv helper

    LDCORESHARED_EXPORT void rgb2hsv(float& r, float& g, float& b, float& h, float& s, float& v);
    LDCORESHARED_EXPORT void hsv2rgb(float& h, float& s, float& v, float& r, float& g, float& b);
    LDCORESHARED_EXPORT void hueSet(float& r, float& g, float& b, float hue);
    LDCORESHARED_EXPORT void hueShift(float& r, float& g, float& b, float hueoffset);
}

class LDCORESHARED_EXPORT ldAbstractColorEffect {

public:
    void setBaseColorDecay(int baseColorDecay);

    virtual uint32_t getColor(const Vec2& p_point, const SvgDim &p_dim) = 0;
    virtual void updateColor();

protected:
    float _millis = 0.0f;

    int _baseColorDecay = 223;
};

class LDCORESHARED_EXPORT ldAbstractStepColorEffect : public ldAbstractColorEffect {

public:
    virtual void updateColor() override;

protected:
    float _lastColorStep = 0.f;
    int _lastSecond = 0;
};

class LDCORESHARED_EXPORT ldColorEffectOne : public ldAbstractStepColorEffect {
public:
    virtual uint32_t getColor(const Vec2& p_point, const SvgDim &p_dim) override;

private:

};
class LDCORESHARED_EXPORT ldColorEffectTwo : public ldAbstractStepColorEffect {
public:

    virtual uint32_t getColor(const Vec2& p_point, const SvgDim &p_dim) override;

private:

};
class LDCORESHARED_EXPORT ldColorEffectThree : public ldAbstractColorEffect {
public:
    virtual uint32_t getColor(const Vec2& p_point, const SvgDim &p_dim) override;

private:

};

#endif // LDCOLORUTIL_H

