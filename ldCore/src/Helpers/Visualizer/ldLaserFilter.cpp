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
//  ldLaserFilter.cpp
//  ldCore
//
//  Created by user  on 7/16/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Visualizer/ldLaserFilter.h"
#include <math.h>

// laserpoint struct constructors and utility funcs

LaserPoint::LaserPoint() : x(0.0f), y(0.0f), r(0.0f), g(0.0f), b(0.0f) {}
LaserPoint::LaserPoint(float _x, float _y, float _r, float _g, float _b) {
    x = _x; y = _y; r = _r; g = _g; b = _b;
}
LaserPoint::LaserPoint(float _x, float _y, float _c) {
    x = _x; y = _y; r = _c; g = _c; b = _c;
}
uint32_t LaserPoint::c32() const{
    int ir = clampf(r,0,1)*255;
    int ig = clampf(g,0,1)*255;
    int ib = clampf(b,0,1)*255;
    {
        uint32_t result = (ir << 16) | (ig << 8) | (ib);

        return result;
    }
}

LaserPoint::LaserPoint(const Vertex &v){
    x = v.position[0];
    y = v.position[1];
    r = v.color[0];
    g = v.color[1];
    b = v.color[2];
}

Vertex LaserPoint::toVertex() const {
    Vertex v;
    v.position[0] = x;
    v.position[1] = y;
    v.position[2] = 0;
    v.color[0] = r;
    v.color[1] = g;
    v.color[2] = b;
    v.color[3] = 1;
    return v;
}

// filters

// rotate about center
LaserPoint filter_rotate(LaserPoint t, float angle) {
    LaserPoint t2 = t;
    t2.x = t.x*sinf(angle) + t.y*cosf(angle);
    t2.y = t.x*cosf(angle) - t.y*sinf(angle);
    return t2;
}

LaserPoint filter_gamma(LaserPoint t, float gamma) {
    float e = 1.0/(gamma);
    t.r = powf(t.r, e);
    t.g = powf(t.g, e);
    t.b = powf(t.b, e);
    return t;
}

// second approximation of appearance function, uses 3 inputs
// b = y-intercept (value when input is zero)
// c = slope
// y = gamma exponent
LaserPoint filter_brightContGamma(LaserPoint p, float b, float c, float y) {
     p = filter_gamma(p, y);
     if (p.r > 0.01) p.r = b + p.r*c;
     if (p.g > 0.01) p.g = b + p.g*c;
     if (p.b > 0.01) p.b = b + p.b*c;
     p.r = clampf(p.r, 0, 1);
     p.g = clampf(p.g, 0, 1);
     p.b = clampf(p.b, 0, 1);
    return p;
}

// filter: ripple
// deforms points to ripple effect and casts a ripple shadow
// combination of ripple position and ripple color effecst
LaserPoint filter_ripple(LaserPoint t, float wave, float wave2, float val3) {
    float fx = t.x;
    float fy = t.y;
    float dist = sqrtf(fx*fx+fy*fy);
    float xmod = dist?fx/dist:0;
    float ymod = dist?fy/dist:0;
    // fx *= (0.5+v1);
    //fy *= (0.5+v1);
    float factor = /*(val1)*0.2 **/ sinf(wave+dist*3*6);
    factor *= val3;
    fx += xmod * factor;
    fy += ymod * factor;
    t.x = fx;
    t.y = fy;
    float factor2 = sinf(wave2-dist*3*3)/2+0.5;
    //factor2 = clampf(factor2, 0.25, 0.99);
    t.r *= factor2;
    t.g *= factor2;
    t.b *= factor2;
    return t;
}

#ifndef M_PI
    #define M_PI 3.141592653589793
#endif

// filter: ripple position
// deforms points to ripple effect
LaserPoint filter_ripplePosition(LaserPoint t, float freq, float offset, float strength) {
    float fx = t.x;
    float fy = t.y;
    float dist = sqrtf(fx*fx+fy*fy);
    float xmod = dist?fx/dist:0;
    float ymod = dist?fy/dist:0;
    float factor = strength * sinf((dist*freq+offset)* M_PI*2);
    factor *= 0.25;
    fx += xmod * factor;
    fy += ymod * factor;
    t.x = fx;
    t.y = fy;
    return t;
}

// filter: ripple color
// fades color in a ripple pattern
LaserPoint filter_rippleColor(LaserPoint t, float freq, float offset, float strength) {
    float fx = t.x;
    float fy = t.y;
    float dist = sqrtf(fx*fx+fy*fy);
    float factor = strength * (sinf((dist*freq+offset)* M_PI*2)/2+0.5) + (1-strength);
    //factor = clampf(factor, 0.01, 0.99);
    t.r *= factor;
    t.g *= factor;
    t.b *= factor;
    return t;
}

// filter: color sync
// lag color signal to compensate for galvo delay.  offset = 4 seems to work best.
LaserPoint filter_colorSync(LaserPoint t, int offset) {
#define maxoffset 16
    bool backwards = false;
    if (offset < 0) {
        backwards = true;
        offset = -offset;
    }/* else
        offset = MIN(maxoffset, offset);
    offset = MAX(    0    , offset);*/
    static LaserPoint lagBuffer[maxoffset];
    static int nextfill = 0;
    lagBuffer[nextfill] = t;
    LaserPoint old = lagBuffer[(nextfill + maxoffset - offset)%maxoffset];
    nextfill = (nextfill + 1) % maxoffset;
    if (!backwards) {
        t.r = old.r;
        t.g = old.g;
        t.b = old.b;
    } else {
        t.x = old.x;
        t.y = old.y;
    }
    return t;
}

// filter: tracer
// traces the path of the input.  requires an extremely slow input signal (~500x) for good effect
// input coordinate = head of snake
#define TRACER_POINTS 256
static const int tracerSkip = 100;
static const int tracerLength = 256-32-32;
static const int tracerResetPoints = 32;
static const int tracerTotalPoints = tracerLength + tracerResetPoints*2;
LaserPoint filter_tracer(LaserPoint t) {
    static LaserPoint tracerBuffer[TRACER_POINTS];
    static int skipper = 0;
    skipper = (skipper + 1) % tracerSkip;
    if (skipper == 0) {
        // add point
        for (int i = 1; i < TRACER_POINTS; i++) {
            tracerBuffer[TRACER_POINTS - i] = tracerBuffer[TRACER_POINTS - i - 1];
        }
        tracerBuffer[0] = t;
    }
    // buffer is full, 0 is most recent point
    static int tracerCounter = 0;
    tracerCounter = (tracerCounter + 1) % tracerTotalPoints;
    if (tracerCounter < tracerLength) {
        // along tracer
        int index = ((float)tracerCounter / tracerLength) * (TRACER_POINTS - 1);
        t = tracerBuffer[index];
    } else if (tracerCounter < (tracerLength + tracerResetPoints)) {
        // end of tracer
        t = tracerBuffer[TRACER_POINTS - 1];
        t.r = t.g = t.b = 0;
    } else {
        // beginning of tracer
        t = tracerBuffer[0];
        t.r = t.g = t.b = 0;
    }
    return t;
}

// wavy filter
// vertical sine waves e.g. image "||||" goes to "SSSS" .
// frequency = number of waves from top to bottom.  offset = move waves up/down.
LaserPoint filter_wavy(LaserPoint t, float frequency, float offset, float width) {
    float angle = (t.y * frequency + offset) * M_PI*2;
    t.x += width * sin(angle);
    //t.x = clampf(t.x, -1, 1);
    return t;
}

// color bands
// horizontal bands of brightness.
// params same as wavy filter.  width:1 = white/black bands.  width:0.5 = lgrey/dgrey bands.
LaserPoint filter_colorBands(LaserPoint t, float frequency, float offset, float width) {
    float angle = (t.y * frequency + offset) * M_PI*2;
    float m = width * sin(angle) / 2 + 0.5;
    t.r *= m;    t.g *= m;    t.b *= m;
    return t;
}

// vignette: fade out towards edges. width = border width, x=fade left/right sides, y=fade top/bottom
LaserPoint filter_vignette(LaserPoint t, float width, bool x, bool y) {
    float m = 1;
    if (x) {
        float absx = fabsf(t.x);
        if (absx > (1-width)) m *= 1-((absx-(1-width))/(width));
       // m = clampf(m, 0, 1);
    }
    if (y) {
        float absy = fabsf(t.y);
        if (absy > (1-width)) m *= 1-((absy-(1-width))/(width));
       // m = clampf(m, 0, 1);
    }
    t.r *= m;    t.g *= m;    t.b *= m;
    return t;
}

// soften: compensate brightness for laser velocity
// unitDistance = length of line that will retain full brightness.  lines shorter than this will be dimmed
// requres a very accurate color correction or it just looks weird.
LaserPoint filter_soften(LaserPoint t, float unitDistance) {
    static float lastx = 0;
    static float lasty = 0;
    float dx = t.x-lastx;
    float dy = t.y-lasty;
    float distance = sqrtf(dx*dx+dy*dy);
    float multiplier = distance / unitDistance;
    // todo: make formula more accurate at low values. for now, clamp at 0.25 to avoid 'too-dark corners'
   // multiplier = clampf(multiplier, 0.15, 0.99);
    //if (multiplier > 1) multiplier = 1;
    t.r *= multiplier;
    t.g *= multiplier;
    t.b *= multiplier;
    lastx = t.x;
    lasty = t.y;
    return t;
}

LaserPoint filter_gradientArray(LaserPoint t, float array[], int arraySize) {
    float x = (t.x + 1) / 2;
    int index = x * arraySize;
    if (index < 0) index = 0;
    if (index > arraySize-1) index = arraySize-1;
    float mult = array[index];
    t.r *= mult;
    t.g *= mult;
    t.b *= mult;
    return t;
}

#include <math.h>
float max(float a, float b) {return (a>b)?a:b;}

LaserPoint filter_circlefy(LaserPoint t, float strength) {
    float radius = sqrtf(t.x*t.x+t.y*t.y);
    float shouldRadius = max(fabsf(t.x), fabsf(t.y));
    if (radius > 0) {
        float factor = shouldRadius / radius;
        factor = (strength)*factor + (1-strength)*1;
        t.x *= factor;
        t.y *= factor;
    }
    return t;
}

LaserPoint filter_quiet(LaserPoint t, float maxDistance) {
    static float lastx = 0;
    static float lasty = 0;
    float dx = t.x - lastx;
    float dy = t.y - lasty;
    float dist = sqrtf(dx*dx+dy*dy);
    if (dist > maxDistance) {
        float mult = maxDistance / dist;
        t.x = lastx + dx*mult;
        t.y = lasty + dy*mult;
    }/*
    if (dx > maxDistance) dx = maxDistance;
    if (dx < -maxDistance) dx = -maxDistance;
    if (dy > maxDistance) dy = maxDistance;
    if (dy < -maxDistance) dy = -maxDistance;
    t.x = lastx + dx;
    t.y = lasty + dy;*/

    lastx = t.x;
    lasty = t.y;
    return t;
}


