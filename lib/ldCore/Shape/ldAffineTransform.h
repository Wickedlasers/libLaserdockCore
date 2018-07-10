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
//  ldAffineTransform.h
//  LaserdockEngine
//
//  Created by rock on 1/6/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef __LaserdockEngine__ldAffineTransform__
#define __LaserdockEngine__ldAffineTransform__

#include "ldCore/Shape/ldParticleGeometry.h"

struct LDCORESHARED_EXPORT ldAffineTransform {
    float a, b, c, d;
    float tx, ty;
};

ldAffineTransform __ldAffineTransformMake(float a, float b, float c, float d, float tx, float ty);
#define ldAffineTransformMake __ldAffineTransformMake

CCPoint __CCPointApplyAffineTransform(const CCPoint& point, const ldAffineTransform& t);
#define CCPointApplyAffineTransform __CCPointApplyAffineTransform

CCSize __CCSizeApplyAffineTransform(const CCSize& size, const ldAffineTransform& t);
#define CCSizeApplyAffineTransform __CCSizeApplyAffineTransform

ldAffineTransform ldAffineTransformMakeIdentity();
//CCRect CCRectApplyAffineTransform(const CCRect& rect, const ldAffineTransform& anAffineTransform);

ldAffineTransform ldAffineTransformTranslate(const ldAffineTransform& t, float tx, float ty);
ldAffineTransform ldAffineTransformRotate(const ldAffineTransform& aTransform, float anAngle);
ldAffineTransform ldAffineTransformScale(const ldAffineTransform& t, float sx, float sy);
ldAffineTransform ldAffineTransformConcat(const ldAffineTransform& t1, const ldAffineTransform& t2);
bool ldAffineTransformEqualToTransform(const ldAffineTransform& t1, const ldAffineTransform& t2);
ldAffineTransform ldAffineTransformInvert(const ldAffineTransform& t);

#endif /* defined(__LaserdockEngine__ldAffineTransform__) */
