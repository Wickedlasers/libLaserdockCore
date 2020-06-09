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

#include <cmath>

#include "ldCore/Helpers/Color/ldColorUtil.h"

// ---------- ldColorCurveFilter ----------

float ldColorCurve::get(float f) {
    // old
    /* float a, b, z;
         if      (f <= 0.25) {a =  0; b = x1; z = (f-0.00)*4;}
         else if (f <= 0.50) {a = x1; b = x2; z = (f-0.25)*4;}
         else if (f <= 0.75) {a = x2; b = x3; z = (f-0.50)*4;}
         else                {a = x3; b =  1; z = (f-0.75)*4;}
         return (1-z)*a + (z)*b;*/

    float midx = (1-dflect)/2;
    float midy = ((1+dflect)/2) * (gain-thold) + thold;

    if (f < midx) {
        float slope = (midy-thold)/(midx-0);
        return  slope*(f-0) + thold;
    } else {
        float slope = (gain-midy)/(1-midx);
        return  slope*(f-midx) + midy;
    }
}

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

void ldColorCurveFilter::process(ldVertex &v)
{
    if(!m_enabled)
        return;

    if (v.r() > 0 || blackUseTHold) v.r() = curveR.get(v.r());
    if (v.g() > 0 || blackUseTHold) v.g() = curveG.get(v.g());
    if (v.b() > 0 || blackUseTHold) v.b() = curveB.get(v.b());

    for (int i = 0; i < ldVertex::COLOR_COUNT; i++) v.color[i] = fminf(fmaxf(v.color[i], 0), 1);
}

// ---------- ldColorFaderFilter ----------

void ldColorFaderFilter::process(ldVertex &v)
{
    v.r() *= r;
    v.g() *= g;
    v.b() *= b;
}


// ---------- ldHueFilter ----------

void ldHueFilter::process(ldVertex &input)
{
    float h, s, v;
    ldColorUtil::colorRGBtoHSVfloat(input.r(), input.g(), input.b(), h, s, v);

    h = m_value;

    // colorize white color too, but save black parts as black
    float minSaturation = 0.5f;
    float saturationCoeff = 1.f - minSaturation;
    s = std::max(minSaturation, s) + 0.;
    s += std::max(v - saturationCoeff, 0.f);
    s = std::min(s, 1.0f);

    ldColorUtil::colorHSVtoRGBfloat(h, s, v, input.r(), input.g(), input.b());
}


// ---------- ldHueMatrixFilter ----------

void ldHueMatrixFilter::process(ldVertex &input)
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
    vr = c1r * input.r() + c2r * input.g() + c3r * input.b();
    vg = c1g * input.r() + c2g * input.g() + c3g * input.b();
    vb = c1b * input.r() + c2b * input.g() + c3b * input.b();
    if (vr < 0) vr = 0;
    if (vr > 1) vr = 1;
    if (vg < 0) vg = 0;
    if (vg > 1) vg = 1;
    if (vb < 0) vb = 0;
    if (vb > 1) vb = 1;
    input.r() = vr; input.g() = vg; input.b() = vb;
}


// ---------- ldHueShiftFilter ----------

void ldHueShiftFilter::process(ldVertex &input)
{
    float hh, ss, vv;
    ldColorUtil::colorRGBtoHSVfloat(input.r(), input.g(), input.b(), hh, ss, vv);
    hh += m_value;
    hh -= (int) hh;
    ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, input.r(), input.g(), input.b());
}

// ---------- ldFlipFilter ----------

void ldFlipFilter::process(ldVertex &v)
{
    // flip xy
    if (flipX) v.x() *= -1;
    if (flipY) v.y() *= -1;
}


// ---------- ldPowerFilter ----------

void ldPowerFilter::process(ldVertex &v)
{
    // global brightness 1
    v.r() *= m_brightness;
    v.g() *= m_brightness;
    v.b() *= m_brightness;

    for (int i = 0; i < ldVertex::COLOR_COUNT; i++)
        v.color[i] = fminf(fmaxf(v.color[i], 0), 1);
}

// ---------- ldRotateFilter ----------

void ldRotateFilter::process(ldVertex &v)
{
    if(!m_enabled)
        return;

    // rotate
    float tx = v.x();
    float ty = v.y();
    float angle = -degCW * M_PI / 180.0f;
    float sinangle = sinf(angle);
    float cosangle = cosf(angle);
    v.y() = tx*sinangle + ty*cosangle;
    v.x() = tx*cosangle - ty*sinangle;
}

// ---------- ld3dRotateFilter ----------

void ld3dRotateFilter::setX(float x)
{
    m_x = x*ROT_RANGE;
}

void ld3dRotateFilter::setY(float y)
{
    m_y = y*ROT_RANGE;
}

void ld3dRotateFilter::process(ldVertex &input)
{
    if(cmpf(m_x, 0) && cmpf(m_y, 0))
        return;

    ldVec3 v3{input.x(), input.y(), 0};
    v3.rotate(ldVec3::Y_VECTOR, m_x);
    v3.rotate(ldVec3::X_VECTOR, m_y);
    input.x() = v3.x;
    input.y() = v3.y;
}

// ---------- ldTracerFilter ------------

void ldTracerFilter::process(ldVertex &input)
{
    //for (int i = 0; i < 4; i++) v.color[i] = 1; // old implementation, just sets everything white
    m_dash = !m_dash;
    if (m_dash)
        if (input.isBlank())
            for (int i = 0; i < ldVertex::COLOR_COUNT; i++) input.color[i] = 1;
}

// ---------- ldScaleFilter ----------

const float SCALE_MIN_VALUE = 0.1f;
const float SCALE_MAX_VALUE = 1.f;

ldScaleFilter::ldScaleFilter()
{

}

void ldScaleFilter::process(ldVertex &v)
{
    if(!m_enabled)
        return;

    v.x() *= xScale();
    v.y() *= yScale();
}

void ldScaleFilter::setXScale(float value)
{
    if (value > SCALE_MAX_VALUE) value = SCALE_MAX_VALUE;
    if (value < SCALE_MIN_VALUE) value = SCALE_MIN_VALUE;

    m_xScale = value;
}

float ldScaleFilter::xScale() const
{
    return m_xScale;
}

void ldScaleFilter::setYScale(float value)
{
    if (value > SCALE_MAX_VALUE) value = SCALE_MAX_VALUE;
    if (value < SCALE_MIN_VALUE) value = SCALE_MIN_VALUE;

    m_yScale = value;
}

float ldScaleFilter::yScale() const
{
    return m_yScale;
}

void ldScaleFilter::setEnabled(bool enabled)
{
    m_enabled = enabled;
}

bool ldScaleFilter::isEnabled() const
{
    return m_enabled;
}

// ---------- ldShiftFilter ----------

ldShiftFilter::ldShiftFilter(std::vector<ldScaleFilter*> scaleFilters)
    : ldFilter()
    , m_scaleFilters(scaleFilters)
{

}

void ldShiftFilter::process(ldVertex &v)
{
    // shift
    float xScale = 1;
    float yScale = 1;
    for(ldScaleFilter *scaleFilter : m_scaleFilters) {
        if(!scaleFilter->isEnabled())
            continue;

        xScale *= scaleFilter->xScale();
        yScale *= scaleFilter->yScale();
    }
    float fsx = x * (1.0f - xScale);
    float fsy = y * (1.0f - yScale);
    v.x() += fsx;
    v.y() += fsy;
}

// ---------- ldTtlFilter ----------

void ldTtlFilter::process(ldVertex &v)
{
    if(!m_enabled)
        return;

    // color hue preserving transformation
    float c1 = 0.25f; // cutoff for K/RGB
    float c2 = 0.50f; // cutoff for RGB/CMY
    float c3 = 0.66f; // cutiff for CMY/W
    int i1 = 0;
    for (int i = 0; i < ldVertex::COLOR_COUNT; i++)
        if (v.color[i] > v.color[i1]) i1 = i;
    int i2 = (i1+1)%ldVertex::COLOR_COUNT;

    for (int i = 0; i < ldVertex::COLOR_COUNT; i++)
        if (i != i1)
            if (v.color[i] > v.color[i2])
                i2 = i;

    int i3 = 0;
    if ((i3 == i1) || (i3 == i2))
        i3++;
    if ((i3 == i1) || (i3 == i2))
        i3++;
    float v1 = v.color[i1];
    float v2 = v.color[i2];
    float v3 = v.color[i3];
    v.clearColor();
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
