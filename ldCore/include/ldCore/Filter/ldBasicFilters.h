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

#ifndef LDBASICFILTERS_H
#define LDBASICFILTERS_H

#include <cmath>

#include <ldCore/ldCore_global.h>
#include <ldCore/Helpers/Maths/ldGlobals.h>

#include "ldFilter.h"

#include <QElapsedTimer>

// ---------- ldColorFilter ----------

/** Replace all colors by only one */
class LDCORESHARED_EXPORT ldColorFilter : public ldFilter {
public:
    virtual void process(ldVertex &v) override;

    bool enabled = false;
    float r = 1.f;
    float g = 1.f;
    float b = 1.f;
};

// ---------- ldColorCurveFilter ----------

// class for color curve
class LDCORESHARED_EXPORT ldColorCurve {
public:
    float get(float f);

    void setGamma(int val); // accepts range 100 (gamma 1.0 for linear) to 800 (gamma of 8.0)
    void setGamma(float val); // accepts range 1.0f to 8.0f
    int  getGamma(); // returns int version of the current gamma (100 to 800)
    float getGammaf(); // returns real version of the current gamma (1.00f to 8.00f)

    void setMin(int val); // set minimum value (0 = 0.0f, 255 = 1.0f)
    void setMin(float val); // set minimum value (0.0f to 1.0f)
    int  getMin(); // get integer value of the current min value (0-255)
    float getMinf(); // returns real value of the current min value (0.0f to 1.0f)

    void setMax(int val); // set max from integer val (0 = 0.0f, 255 = 1.0f)
    void setMax(float val); // set max (0.0f to 1.0f)
    int  getMax(); // returns integer value of the current max value (0-255)
    float getMaxf(); // returns real value of the current max value (0.0f to 1.0f)

private:
    // int versions of min/max/gamma
    int imin = 0;
    int imax = 255;
    int igamma = 220;
    // real values of min/max/gamma
    float minval = 0.0f;
    float maxval = 1.0f;
    float gammaval = 2.2f;


};

/** Smooth filters output colors to the only required one */
class LDCORESHARED_EXPORT ldColorCurveFilter : public ldFilter {
public:
    explicit ldColorCurveFilter();

    virtual void process(ldVertex &v) override;

    bool m_enabled = false;

    ldColorCurve curveR, curveG, curveB;

    bool blackUseTHold = false; // deprecated and can be removed?
};


// ---------- ldColorFaderFilter ----------

/** Smooth filters output colors to the only required one */
class LDCORESHARED_EXPORT ldColorFaderFilter : public ldFilter {
public:
    virtual void process(ldVertex &v) override;

    float r = 1.f;
    float g = 1.f;
    float b = 1.f;
};

// ---------- ldEmptyFilter ----------

/** Empty filter, does nothing */
class LDCORESHARED_EXPORT ldEmptyFilter : public ldFilter {
public:
    virtual void process(ldVertex & /*v*/) override {}
    virtual QString name() override { return tr("No Effect"); }
};


// ---------- ldHueFilter ----------

/** Colorize output to one hue */
class LDCORESHARED_EXPORT ldHueFilter : public ldFilter {
public:
    virtual void process(ldVertex &input) override;
    virtual QString name() override { return QObject::tr("Custom hue basic"); }

    bool m_enabled = false;
    float m_value = 0;
};


// ---------- ldHueMatrixFilter ----------

/** Hue matrix complex filter */
class LDCORESHARED_EXPORT ldHueMatrixFilter : public ldFilter
{
public:
    virtual void process(ldVertex &input) override;

    bool m_enabled = false;
    float m_value = 0.5f; // 0to1

private:
    float c1r, c1g, c1b, c2r, c2g, c2b, c3r, c3g, c3b;
    float m_last = -1.0f;
};


// ---------- ldHueShiftFilter ----------

/** Shift hue filter */
class LDCORESHARED_EXPORT ldHueShiftFilter : public ldFilter
{
public:
    virtual void process(ldVertex &input) override;

    bool m_enabled = false;
    float m_value = 0.5f; // 0to1
};


// ---------- ldFlipFilter ----------

/** Rotate filter */
class LDCORESHARED_EXPORT ldFlipFilter : public ldFilter
{
public:
    virtual void process(ldVertex &v) override;

    bool flipX = false;
    bool flipY = false;
};

// ---------- ldPowerFilter ----------

/** Rotate filter */
class LDCORESHARED_EXPORT ldPowerFilter : public ldFilter
{
public:
    virtual void process(ldVertex &input) override;

    // color
    float m_brightness = 1.f;
};

// ---------- ldStrobeFilter ----------

/** Strobe filter */
class LDCORESHARED_EXPORT ldStrobeFilter : public ldFilter
{
public:
    ldStrobeFilter();
    virtual void process(ldVertex &input) override;

    bool m_enabled = false;

    // frame count (### temporary)
    int m_frame = 0;
    QElapsedTimer etimer;

    // time on
    int m_timeOn = 10;

    // time off
    int m_timeOff = 2;
};

// ---------- ldRotateFilter ----------

/** Rotate filter */
class LDCORESHARED_EXPORT ldRotateFilter : public ldFilter
{
public:
    virtual void process(ldVertex &input) override;

    bool m_enabled = true;
    float degCW = 0.f;
};


/** 3D Rotate filter */
class LDCORESHARED_EXPORT ld3dRotateFilter : public ldFilter
{
public:
    void setX(float x);
    void setY(float y);

protected:
    virtual void process(ldVertex &input) override;

private:
    static constexpr const float ROT_RANGE = M_PIf;

    float m_x = 0;
    float m_y = 0;
};


// ---------- LdTracerFilter ----------

/** Tracer filter adds a dashed line to black areas and helps to see how laser works. Useful for debug */
class LDCORESHARED_EXPORT ldTracerFilter : public ldFilter
{
public:
    virtual QString name() override { return "Debug Trace"; }
    virtual void process(ldVertex &input) override;

    bool m_enabled = false;
private:
    bool m_dash = true;
};

// ---------- ldScaleFilter ----------

const float DEFAULT_MAX_SCALE_VALUE = 1.f;
const float DEFAULT_RELATIVE_SCALE_VALUE = 0.5f;

/** Scale filter*/
class LDCORESHARED_EXPORT ldScaleFilter : public ldFilter {
public:
    explicit ldScaleFilter();

    // scale
    void setXScale(float value); // 0.1..1
    float xScale() const;

    void setYScale(float value); // 0.1..1
    float yScale() const;

    void setScale(float value);

    // enabled
    void setEnabled(bool enabled);
    bool isEnabled() const;

    // ldFilter
    virtual void process(ldVertex &v) override;

private:
    float m_xScale = DEFAULT_MAX_SCALE_VALUE;
    float m_yScale = DEFAULT_MAX_SCALE_VALUE;

    bool m_enabled = true;
};


// ---------- LdShiftFilter ----------

class LDCORESHARED_EXPORT ldShiftFilter : public ldFilter
{
public:
    ldShiftFilter(std::vector<ldScaleFilter*> scaleFilters);

    virtual void process(ldVertex &v) override;

    // shape
    float x = 0.f;
    float y = 0.f;

private:
    std::vector<ldScaleFilter*> m_scaleFilters;
};
// ---------- ldTtlFilter ----------

class LDCORESHARED_EXPORT ldTtlFilter : public ldFilter
{
public:
    virtual void process(ldVertex &v) override;

    bool m_enabled = false;
};

#endif // LDBASICFILTERS_H

