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

// ldMathStat.h
//  Created by Eric Brugère on 4/13/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#ifndef LDMATHSTAT_H
#define LDMATHSTAT_H

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT ldMathStat
{
public:
    static void copyArray(float tab[], float *result, int size);
    static void normalizeBuffer(float *tab, int size, float ratio);
    static int getKeyForMaxFloatValue(float tab[], int size);
    static int countNotNullValues(float tab[], int size);
    static float getMaxFloatValue(float tab[], int size);
    static float getMinFloatValue(float tab[], int size);
    static float getFloatAverage(float tab[], int size);
    static float getFloatNotNullAverage(float tab[], int size);
    static float getFloatDeviation(float tab[], int size, float average);
};

#endif // LDMATHSTAT_H


