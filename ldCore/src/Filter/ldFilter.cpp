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

ldFilter::ldFilter()
    : ldShader ()
    , m_intensity(1)
    , m_min_intensity(0)
    , m_max_intensity(1)
{
}

void ldFilter::processFilter(Vertex &input)
{
    if(m_intensity != 1.f) {
        Vertex source = input;
        process(input);
        normalize(input, source);
    } else {
        process(input);
    }
}

void ldFilter::normalize(Vertex &input, const Vertex &source)
{
    for(int i = 0; i < 3; i++) {
        input.position[i] = ldMaths::normalize(input.position[i], source.position[i], m_intensity);
    }
    for(int i = 0; i < 4; i++) {
        input.color[i] = ldMaths::normalize(input.color[i], source.color[i], m_intensity);
    }
}

void ldFilter::ShaderFunc(float *x, float *y, uint32_t *color) {
    Vertex input;
    input.x() = *x;
    input.y() = *y;
    input.r() = (float) (((*color & 0xFF0000) >> 16) / 255.0f);
    input.g() = (float) (((*color & 0x00FF00) >> 8) / 255.0f);
    input.b() = (float) (((*color & 0x0000FF) >> 0) / 255.0f);

    process(input);

    *x = input.x();
    *y = input.y();
    *color = ldColorUtil::colorRGB(input.r()*255, input.g()*255, input.b()*255);
}
