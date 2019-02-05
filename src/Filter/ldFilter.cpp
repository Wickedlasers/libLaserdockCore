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

#include "ldCore/Filter/ldFilter.h"

#include "ldCore/Helpers/Color/ldColorUtil.h"

void ldFilter::ShaderFunc(float *x, float *y, uint32_t *color) {
    Vertex input;
    input.position[0] = *x;
    input.position[1] = *y;
    input.color[0] =  1;
    input.color[1] =  1;
    input.color[2] =  1;
    input.color[0] =(float) (((*color & 0xFF0000) >> 16) / 255.0f);
    input.color[1] =(float) (((*color & 0x00FF00) >> 8) / 255.0f);
    input.color[2] =(float) (((*color & 0x0000FF) >> 0) / 255.0f);
    process(input);
    *x = input.position[0];
    *y = input.position[1];
    *color = ldColorUtil::colorRGB(input.color[0]*255, input.color[1]*255, input.color[2]*255);
}
