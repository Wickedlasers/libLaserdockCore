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
//  ldLaserFilter.h
//  ldCore
//
//  Created by user  on 7/16/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

/*!

  ldLaserFilter.h
  Contains pixel shader style functions for laser data

  LaserPoint structure contains float precision values for a single sample
  as xyrgb fields.

  Note some filter functions have state and should be called 1:1 with data.

  Example: filter_quiet applies a transformation to the stream of samples
  such that the galvos are not allowed to exceed certain limits of movement.

  float X = 0.1; // param
  for (every sample in frame) {
     sample = filter_quiet(sample, X);
  }



  */

#ifndef __ldCore__ldLaserFilter__
#define __ldCore__ldLaserFilter__

#include <iostream>

#include "ldCore/Visualizations/ldVisualizer.h"

struct LDCORESHARED_EXPORT LaserPoint {
    LaserPoint();
    LaserPoint(float _x, float _y, float _r, float _g, float _b);
    LaserPoint(float _x, float _y, float _c);
    LaserPoint(const Vertex &v);

    Vertex toVertex() const;
    uint32_t c32() const;

    float x;// = 0;
    float y;// = 0;
    float r;// = 1;
    float g;// = 1;
    float b;// = 1;

};

LDCORESHARED_EXPORT LaserPoint filter_rotate(LaserPoint t, float angle);
LDCORESHARED_EXPORT LaserPoint filter_gamma(LaserPoint t, float gamma);
LDCORESHARED_EXPORT LaserPoint filter_brightContGamma(LaserPoint p, float b = 0.35, float c = 0.75, float y = 0.65);
LDCORESHARED_EXPORT LaserPoint filter_ripple(LaserPoint t, float wave, float wave2, float val3);
LDCORESHARED_EXPORT LaserPoint filter_ripplePosition(LaserPoint t, float freq = 1, float offset = 0, float strength = 1);
LDCORESHARED_EXPORT LaserPoint filter_rippleColor(LaserPoint t, float freq = 1, float offset = 0, float strength = 1);
LDCORESHARED_EXPORT LaserPoint filter_colorSync(LaserPoint t, int offset = 4);
LDCORESHARED_EXPORT LaserPoint filter_tracer(LaserPoint t);
LDCORESHARED_EXPORT LaserPoint filter_wavy(LaserPoint t, float frequency, float offset, float width);
LDCORESHARED_EXPORT LaserPoint filter_colorBands(LaserPoint t, float frequency, float offset, float width);
LDCORESHARED_EXPORT LaserPoint filter_vignette(LaserPoint t, float width = 0.5, bool x = true, bool y = true);
LDCORESHARED_EXPORT LaserPoint filter_soften(LaserPoint t, float unitDistance);
LDCORESHARED_EXPORT LaserPoint filter_gradientArray(LaserPoint t, float array[], int arraySize);
LDCORESHARED_EXPORT LaserPoint filter_circlefy(LaserPoint t, float strength = 1);
LDCORESHARED_EXPORT LaserPoint filter_quiet(LaserPoint t, float maxDistance);

#endif /* defined(__ldCore__ldLaserFilter__) */
