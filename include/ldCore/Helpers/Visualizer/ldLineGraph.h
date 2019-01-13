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
//  ldLineGraph.h
//
//  Created by feldspar on 7/16/15.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef ldLineGraph_H
#define ldLineGraph_H

#include <math.h>
#include <QtCore/QString>
#include <ldCore/Sound/ldSoundData.h>
#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/Audio/ldNoiseField.h"
#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Helpers/Visualizer/ldLineGraph.h"

class LDCORESHARED_EXPORT ldLineGraph {
public:
    ldLineGraph(char* n = (char*) "no name");
    static const int range = 12/AUDIO_UPDATE_DELTA_S; // 12 seconds worth of frames
    float floats[range];
    int head;
    bool enabled;
    char* name;
    uint32_t color;
    float zoom;
    void add(float f);
    void draw(ldRendererOpenlase* r);
};

#endif // ldLineGraph_H
