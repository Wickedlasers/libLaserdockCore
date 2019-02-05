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

#include "ldCore/Filter/ldFilterBasicGlobal.h"

#include <math.h>
#include <QtCore/QDebug>
#include "ldCore/Helpers/Color/ldColorUtil.h"


ldFilterBasicGlobal::ldFilterBasicGlobal()
    : ldFilter()
    , m_colorCurveFilter(new ldColorCurveFilter())
    , m_hueFilter(new ldHueFilter())
    , m_hueMatrixFilter(new ldHueMatrixFilter())
    , m_hueShiftFilter(new ldHueShiftFilter())
    , m_soundLevelFilter(new ldSoundLevelFilter())
    , m_tracerFilter(new ldTracerFilter())
{
}

ldFilterBasicGlobal::~ldFilterBasicGlobal()
{
}

void ldFilterBasicGlobal::process(Vertex &v)
{
    if (ttl) {
        // color hue preserving transformation
        float c1 = 0.25f; // cutoff for K/RGB
        float c2 = 0.50f; // cutoff for RGB/CMY
        float c3 = 0.66f; // cutiff for CMY/W
        int i1 = 0;
        for (int i = 0; i < 3; i++) if (v.color[i] > v.color[i1]) i1 = i;
        int i2 = (i1+1)%3;
        for (int i = 0; i < 3; i++) if (i != i1) if (v.color[i] > v.color[i2]) i2 = i;
        int i3 = 0; if ((i3 == i1) || (i3 == i2)) i3++; if ((i3 == i1) || (i3 == i2)) i3++;
        float v1 = v.color[i1];
        float v2 = v.color[i2];
        float v3 = v.color[i3];
        v.color[0] = v.color[1] = v.color[2] = 0;
        if (v1 >= c1) {
            v.color[i1] = 1;
            if (v2 >= v1*c2) {
                v.color[i2] = 1;
                if (v3 >= v1*c3) {
                    v.color[i3] = 1;
                }
            }
        }
    }
    
    if (m_tracerFilter->m_enabled)
        m_tracerFilter->process(v);

    if (m_hueMatrixFilter->m_enabled)
        m_hueMatrixFilter->process(v);

    if(m_hueShiftFilter->m_enabled)
        m_hueShiftFilter->process(v);

    if(m_hueFilter->m_enabled)
        m_hueFilter->process(v);

    if(m_colorCurveFilter->m_enabled)
        m_colorCurveFilter->process(v);

     m_soundLevelFilter->process(v);
}

ldColorCurveFilter *ldFilterBasicGlobal::colorCurveFilter() const
{
    return m_colorCurveFilter.get();
}

ldHueFilter *ldFilterBasicGlobal::hueFilter() const
{
    return m_hueFilter.get();
}

ldHueMatrixFilter *ldFilterBasicGlobal::hueMatrixFilter() const
{
    return m_hueMatrixFilter.get();
}

ldHueShiftFilter *ldFilterBasicGlobal::hueShiftFilter() const
{
    return m_hueShiftFilter.get();
}

ldTracerFilter *ldFilterBasicGlobal::tracerFilter() const
{
    return m_tracerFilter.get();
}

ldSoundLevelFilter *ldFilterBasicGlobal::soundLevelFilter() const
{
    return m_soundLevelFilter.get();
}
