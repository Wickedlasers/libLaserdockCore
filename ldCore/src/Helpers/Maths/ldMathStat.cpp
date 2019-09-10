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

// ldMathStat.cpp
//  Created by Eric Brugère on 4/13/16.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Maths/ldMathStat.h"

#include <math.h>

// copyArray
void ldMathStat::copyArray(float tab[], float *result, int size)
{
    for (int i = 0; i < size; i++) {
        result[i]=tab[i];
    }
}

// normalizeBuffer
void ldMathStat::normalizeBuffer(float *tab, int size, float ratio)
{
    if (ratio==0) ratio=1.0;
    for (int i = 0; i < size; i++) tab[i]=tab[i]/ratio;
}

// getKeyForMaxFloatValue
int ldMathStat::getKeyForMaxFloatValue(float tab[], int size)
{
    if (size==0) return 0;
    float maxValue=tab[0];
    int maxInt=0;
    for (int i=1; i<size; i++) {
        if (tab[i]>maxValue) {
            maxValue=tab[i];
            maxInt=i;
        }
    }
    return maxInt;
}

// countNotNullValues
int ldMathStat::countNotNullValues(float tab[], int size)
{
    int count=0;
    for (int i=0; i<size; i++) {
        if (tab[i]!=0) count++;
    }
    return count;
}

// getMaxFloatValue
float ldMathStat::getMaxFloatValue(float tab[], int size)
{
    if (size==0) return 0;
    float maxValue=tab[0];
    for (int i=1; i<size; i++) {
        if (tab[i]>maxValue) {
            maxValue=tab[i];
        }
    }
    return maxValue;
}

// getMinFloatValue
float ldMathStat::getMinFloatValue(float tab[], int size)
{
    if (size==0) return 0;
    float minValue=tab[0];
    for (int i=1; i<size; i++) {
        if (tab[i]<minValue) {
            minValue=tab[i];
        }
    }
    return minValue;
}

// getFloatAverage
float ldMathStat::getFloatAverage(float tab[], int size)
{
    if (size==0) return 0;
    //
    float sum=0;
    for (int i=0; i<size; i++) sum+=tab[i];
    return sum/size;
}

// getFloatNotNullAverage
float ldMathStat::getFloatNotNullAverage(float tab[], int size)
{
    if (size==0) return 0;
    float sum=0;
    int count=0;
    for (int i=0; i<size; i++) {
        if (fabs(tab[i])>0) {
            sum+=tab[i];
            count++;
        }
    }
    if (count==0) return 0;
    return sum/count;
}

// getFloatDeviation
float ldMathStat::getFloatDeviation(float tab[], int size, float average)
{
    if (size==0) return 0.0;
    float var=0.0;
    for (int i=0; i<size; i++) {
        var+=powf(tab[i]-average, 2.f);
    }
    var=var/size;
    return sqrtf(var);
}


