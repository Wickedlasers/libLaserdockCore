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

// ldFilterMath.cpp
//  Created by 12/14/19.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Maths/ldFilterMath.h"

#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

#include <math.h>
#include <cmath>
#include <float.h>

using namespace ldFilterMath;

float ldFilterMath::cycle01(float f) {return f - floorf(f);}
float ldFilterMath::mirror01(float f) {if (int(floorf(f)) % 2) f = -f; return cycle01(f);}
float ldFilterMath::clamp01(float f) {return clampf(f, 0, 1);}
void ldFilterMath::clamp01p(float &f) {f = clampf(f, 0, 1);}
float ldFilterMath::rand01() {return (float(rand()))/RAND_MAX;}
int ldFilterMath::randi(int n) {return rand()%n;}

void ldFilterMath::rot(float &x, float &y, float rad) {
    float tsin = sinf(rad);
    float tcos = cosf(rad);
    float ny = x*tsin + y*tcos;
    float nx = x*tcos - y*tsin;
    x = nx;
    y = ny;
}
void ldFilterMath::rott(float &x, float &y, float rad) {   rot(x,y,rad*M_PIf*2); }
void ldFilterMath::xy2to1(float &x, float &y) {x = (x+1)/2; y = (y+1)/2;}
void ldFilterMath::xy1to2(float &x, float &y) {x = (x*2)-1; y = (y*2)-1;}
void ldFilterMath::xytile(int nx, int ny, float x, float y, float &fx, float &fy, int &ix, int &iy) {
    x *= nx; ix = int(floorf(x)); fx = x-ix;
    y *= ny; iy = int(floorf(y)); fy = y-iy;
}
void ldFilterMath::xytileinv(int nx, int ny, float &x, float &y, float fx, float fy, int ix, int iy) {
    x = (ix + fx) / nx;
    y = (iy + fy) / ny;
}
float ldFilterMath::rippleEffect(float x, float y, float wavefreq, float waveangle, float radius, float angle) {
    float tx = radius;
    float ty = 0;
    rott(tx, ty, angle);
    x += tx;
    y += ty;
    float dist = sqrtf(x*x+y*y);
    float f = rcost(dist * wavefreq + waveangle);
    return f;
}


float ldFilterMath::vertColorMax(ldVertex& v) {return fmaxf(v.r(), fmaxf(v.g(), v.b()));}
void ldFilterMath::VertexColorScale(ldVertex& v, float f) {
    v.r() *= f; clampfp(v.r(), 0, 1);
    v.g() *= f; clampfp(v.g(), 0, 1);
    v.b() *= f; clampfp(v.b(), 0, 1);
}

void ldFilterMath::ColorBolden(float &r, float &g, float &b, float hue, bool preserveValue) {

    // remember max
    float oldmax = fmaxf(r, fmaxf(g, b));

    // matrix constants
    float w = 0.00f; // nonsecondary to secondary
    float x = 0.65f; // secondary to secondary
    float y = 0.35f; // secondaries to primary
    float z = 1.25f; // primary to primary
    if (preserveValue) {
        w = 0.00f;
        x = 0.25f;
        y = 0.50f;
        z = 1.00f;
    }

    // 3 converted colors
    float r1, g1, b1;
    float r2, g2, b2;
    float r3, g3, b3;
    r1 = z*r + y*g + y*b;
    g1 = w*r + x*g + w*b;
    b1 = w*r + w*g + x*b;
    r2 = x*r + w*g + w*b;
    g2 = y*r + z*g + y*b;
    b2 = w*r + w*g + x*b;
    r3 = x*r + w*g + w*b;
    g3 = w*r + x*g + w*b;
    b3 = y*r + y*g + z*b;
    //clampfp(r1, 0, 1);clampfp(g1, 0, 1);clampfp(b1, 0, 1);
    //clampfp(r2, 0, 1);clampfp(g2, 0, 1);clampfp(b2, 0, 1);
    //clampfp(r3, 0, 1);clampfp(g3, 0, 1);clampfp(b3, 0, 1);

    // calculate phase
    int phase = int(hue * 3);
    float partial = hue * 3 - phase;
    float vv = partial;
    float vc = 1-vv;

    // blend between converted colors
    if (phase == 0) {
        r = vc*r1 + vv*r2;
        g = vc*g1 + vv*g2;
        b = vc*b1 + vv*b2;
    } else if (phase == 1) {
        r = vc*r2 + vv*r3;
        g = vc*g2 + vv*g3;
        b = vc*b2 + vv*b3;
    } else {
        r = vc*r3 + vv*r1;
        g = vc*g3 + vv*g1;
        b = vc*b3 + vv*b1;
    }

    // renormalize value
    if (preserveValue) {
        float newmax = fmaxf(r, fmaxf(g, b));
        //float h2, s2, v2;
        //ldColorUtil::rgb2hsv(r, g, b, h2, s2, v2);
        float mult = 1;
        if (newmax > 0) mult = oldmax / newmax;
        r *= mult; g *= mult; b *= mult;
    }

    // clamp
    clampfp(r, 0, 1);clampfp(g, 0, 1);clampfp(b, 0, 1);

}

void ldFilterMath::ColorPurify(float &r, float &g, float &b, float str) {
    float max = fmaxf(r, fmaxf(g, b));
    r = clamp01(max + (r - max)*str);
    g = clamp01(max + (g - max)*str);
    b = clamp01(max + (b - max)*str);
}

ldVertex ldFilterMath::vlerp(ldVertex v0, ldVertex v1, float ff) {
    ldVertex v;
    float fc = 1-ff;
    v.x() = fc*v0.x() + ff*v1.x();
    v.y() = fc*v0.y() + ff*v1.y();
    v.r() = fc*v0.r() + ff*v1.r();
    v.g() = fc*v0.g() + ff*v1.g();
    v.b() = fc*v0.b() + ff*v1.b();
    return v;
}

void ldFilterMath::vlerpposp(ldVertex& v, ldVertex v0, ldVertex v1, float ff) {
    float fc = 1-ff;
    v.x() = fc*v0.x() + ff*v1.x();
    v.y() = fc*v0.y() + ff*v1.y();
}

float ldFilterMath::normalizeToBPM(float target, float raw) {
    if (raw <= 0) return target;
    float ltarget = log2f(target);
    float lraw = log2f(raw);
    float ldelta = lraw - ltarget;
    float ldelta2 = ldelta - roundf(ldelta);
    float lresult = ltarget + ldelta2;
    float result = exp2f(lresult);
    //float min = target / sqrtf(2);
    //float max = target * sqrtf(2);
    //if (result < min) result = min;
    //if (result > max) result = max;
    return result;
}

ldFilterMath::Perspective::Perspective() {
    xymult = 1;
    zoffset = 0;
    nearclip = 0.25f;
    farclip = 100.0f;
    fade = true;
    fadebasedist = 1;
    fadeuseradius = false;
    fadeuselinear = true;
    fademin = 0.5f;
    fademax = 1.5f;
}

void ldFilterMath::Perspective::apply(ldVertex& v, float x, float y, float z) {
    bool clipped = false;
    z += zoffset;
    if (z < nearclip) {
        z = nearclip;
        clipped = true;
    }
    if (z > farclip) {
        z = farclip;
        clipped = true;
    }
    float sx = x/z;
    float sy = y/z;
    sx *= xymult;
    sy *= xymult;
    if (sx < -1) {sx = -1; clipped = 1;}
    if (sx >  1) {sx =  1; clipped = 1;}
    if (sy < -1) {sy = -1; clipped = 1;}
    if (sy >  1) {sy =  1; clipped = 1;}
    v.x() = sx;
    v.y() = sy;
    if (clipped) {
        v.r() = v.g() = v.b() = 0;
    } else if (fade) {
        float fadedist = z;
        if (fadeuseradius) {
            fadedist = sqrtf(x*x+y*y+z*z);
        }
        float fadenormdist = fadedist / fadebasedist;
        float fadestr = 1.0f / fadenormdist;
        if (!fadeuselinear) fadestr *= fadestr;
        clampfp(fadestr, fademin, fademax);
        VertexColorScale(v, fadestr);
    }
}


ldFilterMath::FramePulse::FramePulse(float _freq, float _value) {freq = _freq; value = _value; tick = true; first = true;}
void ldFilterMath::FramePulse::start(float _freq, float _value) {freq = _freq; value = _value; tick = true; first = true;}
void ldFilterMath::FramePulse::update(float delta) {
    bool holding = (value >= 1);
    value += freq*delta;
    tick = false;
    if (value >= 1) {
        if (gate || (gatemax > 0 && value >= gatemax)) {
            value -= floorf(value);
            if (holding) value = 0;
            tick = true;
            icounter = (icounter + 1) % imax;
        }
    }
    if (first) {first = false; tick = true;}
}

void ldFilterMath::CycleWithJump::update(float delta, bool tryjump) {
    value += freq*delta;
    timeouttimer -= freq*delta;
    if (timeouttimer <= 0) {
        timeouttimer = 0;
        if (tryjump) {
            value += jumpdist;
            timeouttimer = timeout;
        }
    }
    value -= int(value);
}

bool TickDynamic::update(float delta, float f) {
    tick = false;
    timer += delta;
    if (timer > maxtime) timer = maxtime;
    if (timer < mintime) return tick;
    float ftime = (timer - mintime) / (maxtime - mintime);
    float thold = minthold + (maxthold - minthold) * ftime;
    if (f >= thold) {
        tick = true;
        timer = 0;
    }
    return tick;
}

float PulseDecay::update(float delta, float input) {
    value -= freq*delta;
    value = fmaxf(value, 0);
    if (input > value * thold) value = input;
    return value;
}

ldFilterMath::ColorPanelCanvas::ColorPanelCanvas() {init();}
void ldFilterMath::ColorPanelCanvas::init() {
        xx = int(clampf(xx, 0, maxsize-1));
        yy = int(clampf(yy, 0, maxsize-1));
        for (int x = 0; x < xx; x++) {
            for (int y = 0; y < yy; y++) {
                array[x][y] = 0;
            }
        }
}
bool ldFilterMath::ColorPanelCanvas::isint(float f) {return (f <= float(int(f)));}
void ldFilterMath::ColorPanelCanvas::getRandXYModIntCross(int& xm, int& ym) {
    xm = ym = 0;
    int r = randi(4);        if (r == 0) xm = 1;        if (r == 1) xm = -1;        if (r == 2) ym = 1;        if (r == 3) ym = -1;
}
void ColorPanelCanvas::getRandXYModIntRound(int& xm, int& ym) {
    xm = ym = 0;
    int r = randi(4);        if (r == 0) xm = 1;        if (r == 1) xm = -1;        if (r == 2) ym = 1;        if (r == 3) ym = -1;
    r = randi(4);            if (r == 0) xm = 1;        if (r == 1) xm = -1;        if (r == 2) ym = 1;        if (r == 3) ym = -1;
}

void ldFilterMath::ColorPanelCanvas::grow(float delta) {
    for (int x = 0; x < xx; x++) {
        for (int y = 0; y < yy; y++) {
            float f = array[x][y];
            if (!isint(f)) {
                int oldi = int(f);
                f += delta;
                int newi = int(f);
                if (oldi != newi) f = newi;
                if (f >= steps) f = 0;
                array[x][y] = f;
            }
        }
    }
}
void ldFilterMath::ColorPanelCanvas::spread(float delta) {
    int times = int(delta*xx*yy);
    if (times < 1) times = 1;
    for (int i = 0; i < times; i++) spread();
}
void ldFilterMath::ColorPanelCanvas::spread() {
    int x1 = randi(xx);
    int y1 = randi(yy);
    int xm = 0;
    int ym = 0;
    if (round) getRandXYModIntRound(xm, ym); else getRandXYModIntCross(xm, ym);
    int x2 = (x1 + xm + xx) % xx;
    int y2 = (y1 + ym + yy) % yy;
    float a = array[x1][y1];
    float b = array[x2][y2];
    float dist = (b-a);
    if (dist < 0) dist += steps;
    if (dist > (steps/2.0f)) dist = 0;
    if (dist > thresh) {
        if (instant)
            array[x1][y1] = array[x2][y2];
        else if (isint(array[x1][y1])) {
            array[x1][y1] += 0.01f;
            if (array[x1][y1] >= steps) array[x1][y1] -= steps;
        }
    }
}
void ldFilterMath::ColorPanelCanvas::drop() {
    int rx = randi(xx-2)+1;
    int ry = randi(yy-2)+1;
    if (followsteps) {
        array[rx][ry] = nextstep;
        nextstep = (nextstep + 1) % steps;
    } else if (instant) {
        array[rx][ry] = int(array[rx][ry] + 1) % steps;
    } else if (isint(array[rx][ry])) {
        array[rx][ry] += 0.01f;
        if (array[rx][ry] >= steps) array[rx][ry] -= steps;
    }
}
float ldFilterMath::ColorPanelCanvas::getf(float x, float y) {
    clampfp(x, 0, 1);
    clampfp(y, 0, 1);
    int ix = int(clampf(floorf(x*xx), 0, xx-1));
    int iy = int(clampf(floorf(y*yy), 0, yy-1));
    float f = clampf(array[ix][iy] / steps, 0, 1);
    return f;
}
float ldFilterMath::ColorPanelCanvas::getfSmooth(float x, float y) {
    if (x < 0) x += int(x) + 2;
    if (y < 0) y += int(y) + 2;
    x -= float(int(x));
    y -= float(int(y));
    float x2 = x*xx-0.5f+xx;
    float y2 = y*yy-0.5f+yy;
    int ix = int(x2);
    int iy = int(y2);
    float fx = x2-ix;
    float fy = y2-iy;
    ix = (ix + xx) % xx;
    iy = (iy + yy) % yy;
    int ix1 = ix;
    int iy1 = iy;
    int ix2 = (ix + 1) % xx;
    int iy2 = (iy + 1) % yy;
    clampfp(fx, 0, 1);
    clampfp(fy, 0, 1);
    float cfx = 1-fx;
    float cfy = 1-fy;
    float a1 = array[ix1][iy1]/steps;
    float a2 = array[ix2][iy1]/steps;
    float a3 = array[ix2][iy2]/steps;
    float a4 = array[ix1][iy2]/steps;
    float f1 = cfx*cfy;
    float f2 =  fx*cfy;
    float f3 =  fx* fy;
    float f4 = cfx* fy;
    float ff = f1+f2+f3+f4+0.0001f;
    float f = (a1*f1 + a2*f2 + a3*f3 + a4*f4) / ff;
    //f /= steps;
    if (1) {
        float b1 = array[ix1][iy1]/steps; b1 += 0.5f; b1 -= floorf(b1);
        float b2 = array[ix2][iy1]/steps; b2 += 0.5f; b2 -= floorf(b2);
        float b3 = array[ix2][iy2]/steps; b3 += 0.5f; b3 -= floorf(b3);
        float b4 = array[ix1][iy2]/steps; b4 += 0.5f; b4 -= floorf(b4);
//        float f1 = cfx*cfy;
//        float f2 =  fx*cfy;
//        float f3 =  fx* fy;
//        float f4 = cfx* fy;
//        float ff = f1+f2+f3+f4+0.0001f;
        float ft = (b1*f1 + b2*f2 + b3*f3 + b4*f4) / ff;
        float fdist = 0;
        fdist += f1 * fabsf(a1-f);
        fdist += f2 * fabsf(a2-f);
        fdist += f3 * fabsf(a3-f);
        fdist += f4 * fabsf(a4-f);
        float ftdist = 0;
        ftdist += f1 * fabsf(b1-ft);
        ftdist += f2 * fabsf(b2-ft);
        ftdist += f3 * fabsf(b3-ft);
        ftdist += f4 * fabsf(b4-ft);
        if (ftdist < fdist) {
            ft -= 0.5f;
            ft += 1;
            ft -= floorf(ft);
            f = ft;
        }
    }
    clampfp(f, 0, 1);
    return f;
}




ldFilterMath::GridNoise::GridNoise() {
    for (int x = 0; x < msize; x++) {
        for (int y = 0; y < msize; y++) {
            a[x][y] = 0;
        }
    }
}
void ldFilterMath::GridNoise::init(int s) {
    if (s > msize) s = msize;
    if (s < 2) s = 2;
    size = s;
    clear(0);
}
void ldFilterMath::GridNoise::clear(float z) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            a[x][y] = z;
        }
    }
}
void ldFilterMath::GridNoise::noise(float f) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            a[x][y] += rand01() * f;
            clampfp(a[x][y], 0, 1);
        }
    }
}
void ldFilterMath::GridNoise::stripe(float z, bool h, bool v) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (
                    (v && ((x*2+0) == size || (x*2+1) == size))
                    || (h && ((y*2+0) == size || (y*2+1) == size))
                    )
            {
                a[x][y] += z;
                clampfp(a[x][y], 0, 1);
            }
        }
    }
}
void ldFilterMath::GridNoise::plasma(float d, float f) {
    int n = int(size * size * d);
    int x = rand() % size;
    int y = rand() % size;
    for (int i = 0; i < n; i++) {
        int dir = rand() % 4;
        int xx = 0;
        int yy = 0;
        if (dir == 0) xx = 1;
        if (dir == 1) xx = -1;
        if (dir == 2) yy = 1;
        if (dir == 3) yy = -1;
        x += xx;
        y += yy;
        x = (x + size) % size;
        y = (y + size) % size;
        a[x][y] += f;
        clampfp(a[x][y], 0, 1);
    }
}
float ldFilterMath::GridNoise::get(float x, float y, float zoom, float rot) {
    clampfp(zoom, 0.001f, 1000);
    float z = 1.0f/zoom;
    float xx = (x-0.5f)*z;
    float yy = (y-0.5f)*z;
    rot *= M_PIf * 2;
    y = xx * sinf(rot) + yy * cosf(rot);
    x = xx * cosf(rot) - yy * sinf(rot);
    xx = x + 0.5f;
    yy = y + 0.5f;
    return get(xx, yy);
}

float ldFilterMath::GridNoise::get(float x, float y) {
    if (x < 0) x += int(x) + 2;
    if (y < 0) y += int(y) + 2;
    x -= float(int(x));
    y -= float(int(y));
    float x2 = x*size-0.5f+size;
    float y2 = y*size-0.5f+size;
    int ix = int(x2);
    int iy = int(y2);
    float fx = x2-ix;
    float fy = y2-iy;
    ix = (ix + size) % size;
    iy = (iy + size) % size;
    int ix1 = ix;
    int iy1 = iy;
    int ix2 = (ix + 1) % size;
    int iy2 = (iy + 1) % size;
    clampfp(fx, 0, 1);
    clampfp(fy, 0, 1);
    float cfx = 1-fx;
    float cfy = 1-fy;
    float a1 = a[ix1][iy1];
    float a2 = a[ix2][iy1];
    float a3 = a[ix2][iy2];
    float a4 = a[ix1][iy2];
    float f1 = cfx*cfy;
    float f2 =  fx*cfy;
    float f3 =  fx* fy;
    float f4 = cfx* fy;
    float ff = f1+f2+f3+f4+0.0001f;
    float f = (a1*f1 + a2*f2 + a3*f3 + a4*f4) / ff;
    clampfp(f, 0, 1);
    return f;
}

void ldFilterMath::ColorMap::getRGBClamp(float f, float& r, float& g, float& b) {
    clampfp(f, 0, 1);
    getRGB(f, r, g, b);
}
void ldFilterMath::ColorMap::getRGBCycle(float f, float& r, float& g, float& b) {
    f -= floorf(f);
    clampfp(f, 0, 1);
    getRGB(f, r, g, b);
}
void ldFilterMath::ColorMap::getRGBMirror(float f, float& r, float& g, float& b) {
    if (int(floorf(f)) % 2) f = -f;
    f -= floorf(f);
    clampfp(f, 0, 1);
    getRGB(f, r, g, b);
}


void ldFilterMath::ColorMapHSVGradient::getRGB(float f, float& r, float& g, float& b) {
    float c = 1-f;
    float h, s, v;
    h = c*h1 + f*h2;
    s = c*s1 + f*s2;
    v = c*v1 + f*v2;
    h -= floorf(h);
    clampfp(h, 0, 1);
    clampfp(s, 0, 1);
    clampfp(v, 0, 1);
    ldColorUtil::hsv2rgb(h, s, v, r, g, b);
}

void ldFilterMath::ColorMapBPSW::getRGB(float f, float& r, float& g, float& b) {
    r = clampf(f*3-0.03f, 0, 1)*v;
    g = clampf(f*3-1.00f, 0, 1)*v;
    b = clampf(f*3-1.97f, 0, 1)*v;
    if (hue6 == 0) return;
    float* x[3] = {&r, &g, &b};
    float* y[3] = {&r, &g, &b};
    int z0[6] = {0,1,1,2,2,0};
    int z1[6] = {1,0,2,1,0,2};
    int z2[6] = {2,2,0,0,1,1};
    int ihue = MIN(MAX(hue6 % 6, 0), 5);
    y[0] = x[z0[ihue]];
    y[1] = x[z1[ihue]];
    y[2] = x[z2[ihue]];
    *(y[0]) = clampf(f*3-0.03f, 0, 1)*v;
    *(y[1]) = clampf(f*3-1.00f, 0, 1)*v;
    *(y[2]) = clampf(f*3-1.97f, 0, 1)*v;
}

void FilterMathTestClass::func() {}

