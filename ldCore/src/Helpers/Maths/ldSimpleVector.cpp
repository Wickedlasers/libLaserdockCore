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
//  ldSimpleVector.cpp
//
//  A basic 3 dimensional vector class to simplify flock and physics
//  Created by MEO on 26th April 2015
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Maths/ldSimpleVector.h"
#include <stdio.h>

ldSimpleVector::ldSimpleVector(){
}

ldSimpleVector::ldSimpleVector(double inX, double inY, double inZ) {
    x = inX;
    y = inY;
    z = inZ;
}

ldSimpleVector::~ldSimpleVector(){
}

bool ldSimpleVector::isEqual(ldSimpleVector checkVector) {
    return(x == checkVector.x && y == checkVector.y && z == checkVector.z);
}

ldSimpleVector ldSimpleVector::add(ldSimpleVector addVector) {
    return ldSimpleVector(x + addVector.x, y + addVector.y, z + addVector.z);
}

ldSimpleVector ldSimpleVector::subtract(ldSimpleVector subVector) {
    return ldSimpleVector(x - subVector.x, y - subVector.y, z - subVector.z);
}

ldSimpleVector ldSimpleVector::scalarDivide(double divisor) {
    return ldSimpleVector(x / divisor, y / divisor, z / divisor);
}

ldSimpleVector ldSimpleVector::scalarMultiply(double multiplier) {
    return ldSimpleVector(x * multiplier, y * multiplier, z * multiplier);
}

double ldSimpleVector::euclidianDistance(ldSimpleVector distVector) {
    return(sqrt(pow(distVector.x - x, 2) + pow(distVector.y - y, 2) + pow(distVector.z - z, 2)));
}

double ldSimpleVector::length() {
    return(sqrt((x * x) + (y * y) + (z * z)));
}

ldSimpleVector ldSimpleVector::normalise() {
    ldSimpleVector normalisedVector;
    double length = this->length();

    if(length != 0){
        normalisedVector.x = x / length;
        normalisedVector.y = y / length;
        normalisedVector.z = z / length;
    }
    return normalisedVector;
}
