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
//  ldNoiseField.cpp
//  ldCore
//
//  Created by feldspar on 6/30/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Audio/ldNoiseField.h"
#include <math.h>

float rand01() {
    return ((float)rand())/RAND_MAX;
}

float randRange(float a, float b) {
    return a + (b - a) * rand01();
}

float interpolateBilinear(float q11, float q21, float q12, float q22, float x, float y) {
    float x1 = 0;
    float x2 = 1;
    float y1 = 0;
    float y2 = 1;
    float r = 0;
    r += q11 * (x2-x) * (y2-y);
    r += q21 * (x-x1) * (y2-y);
    r += q12 * (x2-x) * (y-y1);
    r += q22 * (x-x1) * (y-y1);
    return r;
}


ldNoiseField::ldNoiseField(int _xsize, int _ysize, float _speed, bool _type, float varmax) {
    set(_xsize, _ysize, _speed, _type, varmax);
}

ldNoiseField::ldNoiseField() {
    set(10, 10, 1, false);
}

void ldNoiseField::set(int _xsize, int _ysize, float _speed, bool _type, float varmax) {
    xsize = _xsize;
    ysize = _ysize;
    speed = _speed;
    type = _type;

    values.resize(xsize*ysize);
    angle.resize(xsize*ysize);
    angled.resize(xsize*ysize);

    for (int i = 0; i < xsize; i++) {
        for (int j = 0; j < ysize; j++) {
            int ix = index(i, j);
            values[ix] = randRange(-1, 1);
            angle[ix] = randRange(0, TAU);
            angled[ix] = randRange(TAU, varmax*TAU) * ((rand()%2)?1:-1);
        }
    }

    update(0);

}


void ldNoiseField::update(float delta) {
    for (int i = 0; i < xsize; i++) {
        for (int j = 0; j < ysize; j++) {
            if (type) {
                angleIncrement(angle[index(i, j)], angled[index(i, j)]*speed*delta);
                values[index(i, j)] = sinf(angle[index(i, j)]);
            } else {
                float ead = sqrtf(delta*speed*TAU/4);
                if (rand()%2) ead *= -1;
                values[index(i, j)] += ead;
                clampf(values[index(i, j)], -1, 1);
            }
        }
    }
}

float ldNoiseField::get(float x, float y) {
    //return 0;

    float fx = x * (xsize-1);
    int ix1 = MIN(MAX(0, floorf(fx+0)), xsize-1);
    int ix2 = MIN(MAX(0, floorf(fx+1)), xsize-1);
    float xx = MIN(MAX(0, fx-ix1), 1);

    float fy = y * (ysize-1);
    int iy1 = MIN(MAX(0, floorf(fy+0)), ysize-1);
    int iy2 = MIN(MAX(0, floorf(fy+1)), ysize-1);
    float yy = MIN(MAX(0, fy-iy1), 1);

    float tt = interpolateBilinear(
                values[index(ix1, iy1)],
            values[index(ix2, iy1)],
            values[index(ix1, iy2)],
            values[index(ix2, iy2)], xx, yy);
    return tt;

}

int ldNoiseField::index(int x, int y) {
    return x*ysize+y;
}


static void wrap2di(int xsize, int ysize, int& tx, int& ty) {
    if (tx < 0) tx += xsize;
    if (ty < 0) ty += ysize;
    if (tx >= xsize) tx -= xsize;
    if (ty >= ysize) ty -= ysize;
}

static void wrap2df(int xsize, int ysize, float& tx, float& ty) {
    tx /= xsize;
    tx -= floorf(tx);
    tx *= xsize;

    ty /= ysize;
    ty -= floorf(ty);
    ty *= ysize;

    if (tx < 0) tx += xsize;
    if (ty < 0) ty += ysize;
    if (tx >= xsize) tx -= xsize;
    if (ty >= ysize) ty -= ysize;
}


ldPlasmaField::ldPlasmaField(int txsize, int tysize, float tdensity, float tcount) {
    set(txsize, tysize, tdensity, tcount);
}

ldPlasmaField::ldPlasmaField() {
    set(5, 5, 0.5f, 100);
}

void ldPlasmaField::set (int txsize, int tysize, float tdensity, float tcount) {
    xsize = MIN(xsize, ldPlasmaField::maxsquare);
    ysize = MIN(ysize, ldPlasmaField::maxsquare);
    xsize = txsize;
    ysize = tysize;
    density = tdensity;
    count = tcount;

    //values.resize(xsize*ysize);

    for (int i = 0; i < xsize; i++) {
        for (int j = 0; j < ysize; j++) {
            int ix = index(i, j);
            values[ix] = 0;
        }
    }

    float mag = density * xsize * ysize / count;
    float tx = randRange(0.1f, xsize-0.1f);
    float ty = randRange(0.1f, ysize-0.1f);
    for (int k = 0; k < count; k++) {

        float rad = 1;
        float angle = randRange(0, TAU);

        tx += rad*cosf(angle);
        ty += rad*sinf(angle);

        wrap2df(xsize, ysize, tx, ty);

        int ix = index(tx, ty);
        values[ix] += mag;
        clampfp(values[ix], 0, 1);
    }

}



float ldPlasmaField::get(float x, float y) {
    x -= floorf(x);
    y -= floorf(y);

    float fx = x * (xsize);
    int ix1 = MIN(MAX(0, floorf(fx+0)), xsize-1);
    int ix2 = (ix1+1)%xsize;//MIN(MAX(0, floorf(fx+1)), xsize-1);
    float xx = MIN(MAX(0, fx-ix1), 1);

    float fy = y * (ysize);
    int iy1 = MIN(MAX(0, floorf(fy+0)), ysize-1);
    int iy2 = (iy1+1)%ysize;//MIN(MAX(0, floorf(fy+1)), ysize-1);
    float yy = MIN(MAX(0, fy-iy1), 1);

    float tt = interpolateBilinear(
                values[index(ix1, iy1)],
            values[index(ix2, iy1)],
            values[index(ix1, iy2)],
            values[index(ix2, iy2)], xx, yy);
    return tt;

}

int ldPlasmaField::index(int x, int y) {
    wrap2di(xsize, ysize, x, y);
    return x*ysize+y;
}


ldPlasmaNebula::ldPlasmaNebula () {
    set(4, 4, 1.0f, 100, 4, 1, 1);
}

ldPlasmaNebula::ldPlasmaNebula (int txsize, int tysize, float tdensity, float tcount, int tstacks, float trise, float tzoom) {
    set(txsize, tysize, tdensity, tcount, tstacks, trise, tzoom);
}

void ldPlasmaNebula::set (int txsize, int tysize, float tdensity, float tcount, int tstacks, float trise, float tzoom) {
    xsize = txsize;
    ysize = tysize;
    density = tdensity;
    count = tcount;
    stacks = tstacks;
    rise = trise;
    zoom = tzoom;

    fields.resize(stacks);
    for (uint i = 0; i < fields.size(); i++) {
        //int xsize2 = roundf(xsize + randRange(-1.5, 1.5));
        //int ysize2 = roundf(ysize + randRange(-1.5, 1.5));
        int xsize2 = xsize;
        int ysize2 = ysize;
        //if (rand()%2) xsize2++; else ysize2++;
        float density2 = density * randRange(0.66f, 1.5f);
        int count2 = count * randRange(0.66f, 1.5f);
        fields[i].set(xsize2, ysize2, density2, count2);
    }

    fadecounter = 0;
    risecounter = 0;

}


float ldPlasmaNebula::get(float x, float y) {

    float r = 0;

    for (int i = 0; i < stacks; i++) {
        float p = (i + 0.5f) / stacks;

        // fade
        p += fadecounter;
        p -= (int) p;
        p -= 0.5f;
        float fade = 1-(sqrt(p*p) / 0.5f);

        fade = clampf(fade * 2 - 1, 0, 1);
        if (fade <= 0.01f) continue;


        // zoom and rise
        float xx = x;
        float yy = y;

        // rise
        yy -= rise*risecounter;
        yy -= floorf(yy);

        // zoom
        float z = 1 - p * zoom;
        xx = z*(xx-0.5f)+0.5f;
        yy = z*(yy-0.5f)+0.5f;

        r += fields[i].get(xx,  yy) * fade * z;

    }

    r = r*4/stacks; // normalize
    return clampf(r, 0, 1);

}

