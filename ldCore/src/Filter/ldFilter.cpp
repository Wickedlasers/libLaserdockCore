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

#include <ldCore/Filter/ldFilterProcessor.h>
#include <ldCore/Helpers/Color/ldColorUtil.h>

void ldFilter::processFilter(ldVertex &input)
{
    if(m_processorFilter)  {
        m_processorFilter->processFilter(this, input);
    } else {
        process(input);
    }
}

void ldFilter::setProcessorFilter(ldFilterProcessor *processorFilter)
{
    m_processorFilter = processorFilter;
}

void ldFilter::ShaderFunc(float *x, float *y, uint32_t *color) {
    ldVertex input;
    input.x() = *x;
    input.y() = *y;
    input.r() = ((*color & 0xFF0000) >> 16) / 255.0f;
    input.g() = ((*color & 0x00FF00) >> 8) / 255.0f;
    input.b() = ((*color & 0x0000FF) >> 0) / 255.0f;

    process(input);

    *x = input.x();
    *y = input.y();
    *color = ldColorUtil::colorRGBf(input.r(), input.g(), input.b());
}
