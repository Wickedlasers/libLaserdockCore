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

// ldFilterMath.h
//  Created by 12/14/19.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#ifndef LDFILTERMATH_H
#define LDFILTERMATH_H

#include "ldCore/Helpers/Maths/ldGeometryUtil.h"
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Visualizer/ldLaserFilter.h"

#include <math.h>

namespace ldFilterMath
{

    // util functions
    LDCORESHARED_EXPORT float cycle01(float f);
    LDCORESHARED_EXPORT float mirror01(float f);
    LDCORESHARED_EXPORT float clamp01(float f);
    LDCORESHARED_EXPORT void clamp01p(float &f);
    LDCORESHARED_EXPORT float rand01();
    LDCORESHARED_EXPORT int randi(int n);

    LDCORESHARED_EXPORT void rot(float &x, float &y, float rad);
    LDCORESHARED_EXPORT void rott(float &x, float &y, float rad);
    LDCORESHARED_EXPORT void xy2to1(float &x, float &y);
    LDCORESHARED_EXPORT void xy1to2(float &x, float &y);
    LDCORESHARED_EXPORT void xytile(int nx, int ny, float x, float y, float &fx, float &fy, int &ix, int &iy);
    LDCORESHARED_EXPORT void xytileinv(int nx, int ny, float &x, float &y, float fx, float fy, int ix, int iy);
    LDCORESHARED_EXPORT float rippleEffect(float x, float y, float wavefreq, float waveangle, float radius, float angle);

    // color util
    LDCORESHARED_EXPORT float vertColorMax(ldVertex& v);
    LDCORESHARED_EXPORT void VertexColorScale(ldVertex& v, float f);
    LDCORESHARED_EXPORT void ColorBolden(float &r, float &g, float &b, float hue, bool preserveValue);
    LDCORESHARED_EXPORT void ColorPurify(float &r, float &g, float &b, float str);

    // vertex lerp
    LDCORESHARED_EXPORT ldVertex vlerp(ldVertex v0, ldVertex v1, float ff);
    LDCORESHARED_EXPORT void vlerpposp(ldVertex& v, ldVertex v0, ldVertex v1, float ff); //set v's position to a lerp

    // convert a tempo to octave near target
    LDCORESHARED_EXPORT float normalizeToBPM(float target, float raw);

    // perspective transform utility
    class LDCORESHARED_EXPORT Perspective {
    public:
        Perspective();
        float xymult = 1;
        float zoffset = 0;
        float nearclip = 0.25f;
        float farclip = 100.0f;
        bool fade = true;
        float fadebasedist = 1;
        bool fadeuseradius = false;
        bool fadeuselinear = true;
        float fademin = 0.5f;
        float fademax = 1.5f;
        void apply(ldVertex& v, float x, float y, float z);
    };

    // timer utilities
    class LDCORESHARED_EXPORT FramePulse {
    public:
        float value = 0;
        float freq = 1;
        bool tick = true;
        bool first = true;
        bool gate = true;
        float gatemax = 4.0f;
        FramePulse(float _freq = 1, float _value = 0);
        void start(float _freq = 1, float _value = 0);
        void update(float delta);
        int icounter = 0;
        int imax = 2;
    };

    class LDCORESHARED_EXPORT CycleWithJump {
    public:
        float value = 0;
        float freq = 1;
        float timeout = 1;
        float timeouttimer = 0;
        float jumpdist = 0.5f;
        void update(float delta, bool tryjump);        
    };

    class LDCORESHARED_EXPORT TickDynamic {
    public:
        float mintime = 4;
        float maxtime = 16;
        float minthold = 0;
        float maxthold = 1;
        bool tick = false;
        float timer = 0;
        bool update(float delta, float f) ;
    };

    class LDCORESHARED_EXPORT PulseDecay {
    public:
        float value = 0;
        float freq = 2;
        float thold = 2;
        float update(float delta, float input);
    };

    // color array based effects
    class LDCORESHARED_EXPORT ColorPanelCanvas {
    public:
        static const int maxsize = 16;
        int xx = 8;
        int yy = 8;
        int steps = 5;
        bool instant = true;
        bool followsteps = true;
        float thresh = 0;
        bool round = false;
        float array[maxsize][maxsize];
        int nextstep = 1;
        ColorPanelCanvas();
        void init();
        bool isint(float f);
        void getRandXYModIntCross(int& xm, int& ym);
        void getRandXYModIntRound(int& xm, int& ym);
        void grow(float delta);
        void spread(float delta);
        void spread();
        void drop();
        float getf(float x, float y);
        float getfSmooth(float x, float y);
    };

    class LDCORESHARED_EXPORT GridNoise {
    public:
        static const int msize = 32;
        float a[msize][msize];
        int size = msize;
        GridNoise();
        void init(int s);
        void clear(float z = 0);
        void noise(float f = 1);
        void stripe(float z = 1, bool h = true, bool v = true);
        void plasma(float d = 2.5f, float f = 0.2f);
        float get(float x, float y, float zoom, float rot = 0);
        float get(float x, float y);
    };

    // color transforms
    class LDCORESHARED_EXPORT ColorMap {
    public:
        virtual ~ColorMap() = default;

        virtual void getRGB(float f, float& r, float& g, float& b) = 0;
        void getRGBClamp(float f, float& r, float& g, float& b);
        void getRGBCycle(float f, float& r, float& g, float& b);
        void getRGBMirror(float f, float& r, float& g, float& b);
    };
    class LDCORESHARED_EXPORT ColorMapHSVGradient : public ColorMap {
    public:
        float h1 = 0, s1 = 0, v1 = 1;
        float h2 = 0, s2 = 0, v2 = 1;
        virtual void getRGB(float f, float& r, float& g, float& b);
    };
    class LDCORESHARED_EXPORT ColorMapBPSW : public ColorMap {
    public:
        float v = 1;
        int hue6 = 0;
        virtual void getRGB(float f, float& r, float& g, float& b);
    };
}


class LDCORESHARED_EXPORT FilterMathTestClass {
public:
    void func();
};

#endif // LDFILTERMATH


