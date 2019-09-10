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
//  ldNoiseField.h
//  ldCore
//
//  Created by feldspar on 6/30/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldNoiseField__
#define __ldCore__ldNoiseField__

#include <QtCore/QVector>
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

// noise field class
// manages a 2d grid of random variables
// expresses noise as a plasma (random walk) or harmonic oscillator (sin)
// facilitates interpolation between points
// call update(delta) each update frame to animate grid if desired
// call get(x, y) to query the value at a locaiton (0 <= x,y <= 1), returns a float from -1 to 1.

class LDCORESHARED_EXPORT ldNoiseField {
public:

    ldNoiseField(int _xsize, int _ysize, float _speed, bool _type, float varmax = 2);
    ldNoiseField();
    void set(int _xsize, int _ysize, float _speed, bool _type, float varmax = 2);

    int xsize;
    int ysize;
    float speed;
    bool type;

    QVector<float> values;

    QVector<float> angle;
    QVector<float> angled;

    void update(float delta);
    float get(float x, float y);

private:
    int index(int x, int y);

};

LDCORESHARED_EXPORT float rand01();
LDCORESHARED_EXPORT float randRange(float a, float b);

LDCORESHARED_EXPORT float interpolateBilinear(float q11, float q21, float q12, float q22, float x, float y) ;


class LDCORESHARED_EXPORT ldPlasmaField {
public:
    ldPlasmaField();
    ldPlasmaField(int txsize, int tysize, float tdensity, float tcount);
    void set (int txsize, int tysize, float tdensity, float tcount);
    float get(float x, float y);
    //private:
    int xsize;
    int ysize;
    float density;
    int count;
    //QVector<float> values;
    //std::vector<float> values;
    static const int maxsquare = 16;
    float values[maxsquare*maxsquare];
    int index(int x, int y);
};


class LDCORESHARED_EXPORT ldPlasmaNebula {
public:
    ldPlasmaNebula();
    ldPlasmaNebula (int txsize, int tysize, float tdensity, float tcount, int tstacks, float trise, float tzoom);
    void set (int txsize, int tysize, float tdensity, float tcount, int tstacks, float trise, float tzoom);
    float get(float x, float y);

    int xsize;
    int ysize;
    float density;
    int count;
    int stacks;
    float rise;
    float zoom;

    float fadecounter; ///
    float risecounter; ///


//	QVector<ldPlasmaField> fields;
    std::vector<ldPlasmaField> fields;


};


#endif /* defined(__ldCore__ldNoiseField__) */
