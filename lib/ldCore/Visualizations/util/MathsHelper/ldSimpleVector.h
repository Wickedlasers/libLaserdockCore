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

#ifndef LDSIMPLEVECTOR_H
#define LDSIMPLEVECTOR_H

#include <math.h>

#include "ldCore/ldCore_global.h"

//
//  Created by MEO on 26th April 2015
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

class LDCORESHARED_EXPORT ldSimpleVector
{
public:
    ldSimpleVector();
    ~ldSimpleVector();
    double x = 0.0, y = 0.0, z = 0.0;

    ldSimpleVector(double inX, double inY, double inZ);

    bool isEqual(ldSimpleVector checkVector);
    ldSimpleVector add(ldSimpleVector addVector);
    ldSimpleVector subtract(ldSimpleVector subVector);
    ldSimpleVector scalarDivide(double divisor);
    ldSimpleVector scalarMultiply(double multiplier);
    double euclidianDistance(ldSimpleVector distVector);
    double length();
    ldSimpleVector normalise();
};

#endif // LDSIMPLEVECTOR_H
