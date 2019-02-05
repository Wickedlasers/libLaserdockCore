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

#ifndef LDCOLOREFFECTS_H
#define LDCOLOREFFECTS_H

#include <QtCore/QtGlobal>

#include "ldCore/Shape/ldParticleGeometry.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

class LDCORESHARED_EXPORT ldAbstractColorEffect {

public:
    virtual ~ldAbstractColorEffect();

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

#endif // LDCOLOREFFECTS_H

