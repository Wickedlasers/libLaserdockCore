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

#include "ldCore/Filter/ldBasicFilters.h"

#include <math.h>

#include "ldCore/Helpers/Color/ldColorUtil.h"

// ---------- ldColorCurveFilter ----------

ldColorCurveFilter::ldColorCurveFilter()
{
    curveR.gain = (int)(255 * 0.91) / 255.0f;
    curveG.gain = (int)(255 * 0.99) / 255.0f;
    curveB.gain = (int)(255 * 0.48) / 255.0f;
    curveR.thold = (int)(255 * 0.0) / 255.0f;
    curveG.thold = (int)(255 * 0.0) / 255.0f;
    curveB.thold = (int)(255 * 0.0) / 255.0f;
    curveR.dflect = (int)(100 * 0) / 100.0f;
    curveG.dflect = (int)(100 * 0) / 100.0f;
    curveB.dflect = (int)(100 * 0) / 100.0f;
}

void ldColorCurveFilter::process(Vertex &v)
{
    if (v.color[0] > 0 || blackUseTHold) v.color[0] = curveR.get(v.color[0]);
    if (v.color[1] > 0 || blackUseTHold) v.color[1] = curveG.get(v.color[1]);
    if (v.color[2] > 0 || blackUseTHold) v.color[2] = curveB.get(v.color[2]);
}


// ---------- ldHueFilter ----------

void ldHueFilter::process(Vertex &input)
{
    float h, s, v;
    ldColorUtil::colorRGBtoHSVfloat(input.color[0], input.color[1], input.color[2], h, s, v);

    h = m_value;

    // colorize white color too, but save black parts as black
    float minSaturation = 0.5f;
    float saturationCoeff = 1.f - minSaturation;
    s = std::max(minSaturation, s) + 0.;
    s += std::max(v - saturationCoeff, 0.f);
    s = std::min(s, 1.0f);

    ldColorUtil::colorHSVtoRGBfloat(h, s, v, input.color[0], input.color[1], input.color[2]);
}


// ---------- ldHueMatrixFilter ----------

void ldHueMatrixFilter::process(Vertex &input)
{
    float hue = m_value;
    float hh, ss, vv;
    hh = hue;
    ss = 1;
    vv = 1;
    // recalc base colors
    if (m_last != m_value) {
        m_last = m_value;
        float spread = 0.1f; // hue spread
        hh = hue - spread + 1; hh -= (int)hh; ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, c1r, c1g, c1b);
        hh = hue          + 1; hh -= (int)hh; ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, c2r, c2g, c2b);
        hh = hue + spread + 1; hh -= (int)hh; ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, c3r, c3g, c3b);
    }
    float vr, vg, vb;
    vr = c1r * input.color[0] + c2r * input.color[1] + c3r * input.color[2];
    vg = c1g * input.color[0] + c2g * input.color[1] + c3g * input.color[2];
    vb = c1b * input.color[0] + c2b * input.color[1] + c3b * input.color[2];
    if (vr < 0) vr = 0;
    if (vr > 1) vr = 1;
    if (vg < 0) vg = 0;
    if (vg > 1) vg = 1;
    if (vb < 0) vb = 0;
    if (vb > 1) vb = 1;
    input.color[0] = vr; input.color[1] = vg; input.color[2] = vb;
}


// ---------- ldHueShiftFilter ----------

void ldHueShiftFilter::process(Vertex &input)
{
    float hh, ss, vv;
    ldColorUtil::colorRGBtoHSVfloat(input.color[0], input.color[1], input.color[2], hh, ss, vv);
    hh += m_value;
    hh -= (int) hh;
    ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, input.color[0], input.color[1], input.color[2]);
}

// ---------- ldFlipFilter ----------

void ldFlipFilter::process(Vertex &v)
{
    // flip xy
    if (flipX) v.position[0] *= -1;
    if (flipY) v.position[1] *= -1;
}


// ---------- ldRotateFilter ----------

void ldRotateFilter::process(Vertex &v)
{
    if(!m_enabled)
        return;

    // rotate
    float tx = v.position[0];
    float ty = v.position[1];
    float angle = -degCW * M_PI / 180.0f;
    float sinangle = sinf(angle);
    float cosangle = cosf(angle);
    v.position[1] = tx*sinangle + ty*cosangle;
    v.position[0] = tx*cosangle - ty*sinangle;
}


// ---------- ldTracerFilter ------------

void ldTracerFilter::process(Vertex &input)
{
    //for (int i = 0; i < 4; i++) v.color[i] = 1; // old implementation, just sets everything white
    static bool dash = true; dash = !dash;
    if (dash)
        if (input.color[0] == 0 && input.color[1] == 0 && input.color[2] == 0)
            for (int i = 0; i < 4; i++) input.color[i] = 1;
}

// ---------- ldScaleFilter ----------

const float SCALE_MIN_VALUE = 0.1f;
const float SCALE_MAX_VALUE = 1.f;

ldScaleFilter::ldScaleFilter()
{

}

void ldScaleFilter::setRelativeScaleActive(bool active)
{
    m_relativeScaleActive = active;
}

void ldScaleFilter::process(Vertex &v)
{
    v.position[0] *= xScale();
    v.position[1] *= yScale();
}

void ldScaleFilter::setRelativeScale(float value)
{
    if (value > SCALE_MAX_VALUE) value = SCALE_MAX_VALUE;
    if (value < SCALE_MIN_VALUE) value = SCALE_MIN_VALUE;

    m_relative = value;
}

void ldScaleFilter::setMaxXScale(float value)
{
    if (value > SCALE_MAX_VALUE) value = SCALE_MAX_VALUE;
    if (value < SCALE_MIN_VALUE) value = SCALE_MIN_VALUE;

    m_maxX = value;
}

void ldScaleFilter::setMaxYScale(float value)
{
    if (value > SCALE_MAX_VALUE) value = SCALE_MAX_VALUE;
    if (value < SCALE_MIN_VALUE) value = SCALE_MIN_VALUE;

    m_maxY = value;
}

float ldScaleFilter::xScale() const
{
    return m_relativeScaleActive ?
                m_maxX * m_relative :
                m_maxX;
}

float ldScaleFilter::yScale() const
{
    return m_relativeScaleActive ?
                m_maxY * m_relative :
                m_maxY;
}


ldShiftFilter::ldShiftFilter(ldScaleFilter *scaleFilter)
    : ldFilter()
    , m_scaleFilter(scaleFilter)
{

}

void ldShiftFilter::process(Vertex &v)
{
    // shift
    float fsx = x * (1.0f - m_scaleFilter->xScale());
    float fsy = y * (1.0f - m_scaleFilter->yScale());
    v.position[0] += fsx;
    v.position[1] += fsy;
}

