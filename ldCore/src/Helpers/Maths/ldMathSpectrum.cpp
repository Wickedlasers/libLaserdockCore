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

// ldMathSpectrum.cpp
// Created by Eric Brugère on 9/24/16.
// Copyright (c) 2016 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Maths/ldMathSpectrum.h"
#include <QtCore/QDebug>

// getAverageSpectrumValueFromInterval
std::vector<float> ldMathSpectrum::getAverageSpectrumValueFromInterval(ldSoundData* pSoundData, std::vector<int> interval, int nbBandSoundData)
{
    std::vector<float> res;
    for (uint i=0; i<interval.size(); i++)
    {
        int start_interval = 0;
        if (i>0) start_interval = interval[i-1];
        int end_interval = interval[i];
        float tmp = 0.0;
        for (int k=start_interval; k<end_interval; k++)
        {
            tmp += pSoundData->powerAt(k, nbBandSoundData);
            //qDebug() << "pSoundData->powerAt(k, nbBandSoundData)" << pSoundData->powerAt(k, nbBandSoundData);
        }
        //qDebug() << "start " << start_interval  << "   end " << end_interval << "tmp" << tmp;
        if (end_interval > start_interval)  tmp /= (end_interval-start_interval);
        res.push_back(tmp);
    }
    return res;
}

// getNormalizedSpectrumValueFromInterval
std::vector<float> ldMathSpectrum::getNormalizedSpectrumValueFromInterval(ldSoundData* pSoundData, std::vector<int> interval, int nbBandSoundData)
{
    std::vector<float> res;
    std::vector<float> resNormed;
    float max = -1.0;
    for (uint i=0; i<interval.size(); i++)
    {
        int start_interval = 0;
        if (i>0) start_interval = interval[i-1];
        int end_interval = interval[i];
        float tmp = 0.0;
        for (int k=start_interval; k<end_interval; k++)
        {
            tmp += pSoundData->powerAt(k, nbBandSoundData);
            //qDebug() << "pSoundData->powerAt(k, nbBandSoundData)" << pSoundData->powerAt(k, nbBandSoundData);
        }
        //qDebug() << "start " << start_interval  << "   end " << end_interval << "tmp" << tmp;
        res.push_back(tmp);
        if (tmp > max) max = tmp;
    }
    for (uint i=0; i<interval.size(); i++)
    {
        if (max>0) resNormed.push_back(res.at(i)/max);
        else resNormed.push_back(0);
    }
    res.clear();
    return resNormed;
}



