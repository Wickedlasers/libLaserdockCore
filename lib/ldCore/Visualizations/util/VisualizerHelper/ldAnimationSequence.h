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

#ifndef _ldAnimationSequence_h
#define _ldAnimationSequence_h

#include <stdlib.h>
#include <vector>

#include <ldCore/Filter/ldColorUtils.h>
#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"
#include "ldCore/Visualizations/util/SVGHelper/ldSvgReader.h"
#include "ldCore/Visualizations/util/VisualizerHelper/ldLaserFilter.h"

class LDCORESHARED_EXPORT ldAnimationSequence {
public:
    int keyStart[8];
    int keyEnd[8];
    float sourceFPS;
    float sourceBPM;
    virtual void drawFrame(ldRendererOpenlase* r, int index);
};

class LDCORESHARED_EXPORT ldAnimationSequenceBezier : public ldAnimationSequence {
public:
    ldAnimationSequenceBezier();

    svgBezierCurvesSequence frames;
    int renderAlg = 0;
    uint32_t color;

    virtual void drawFrame(ldRendererOpenlase* r, int index) override;  
    void drawFrameLights5(ldRendererOpenlase* r, int index);
    void drawFrameLine4(ldRendererOpenlase* r, int index);    
    void drawFrameBezier3(ldRendererOpenlase* r, int index);
        
// laser draw effect
	void drawFrameBezier3x(ldRendererOpenlase* r, int index, float fstart, float fend, uint32_t c1, uint32_t c2);
	float _fstart;
    float _fend;
// 2 colors and shaders for foreground/background in laser draw
    uint32_t _c1;
    uint32_t _c2;
    ldShader* _shader1;
    ldShader* _shader2;

//load svg sequence
    void load(int i, int j, const char* str);

//.ldva1 format
    void load(const char* str);
    void save(const char* str);

//.ldva2 format
    void load2(const char* str);
    void save2(const char* str);

//.ldva3 format
    void load3(const char* str);
    void save3(const char* str);

// util funcs
    void autoscale();
    void removeblank();
    void scale(float xs, float ys);
    void move(float xt, float yt);
    
};



#endif //#ifndef _ldAnimationSequence_h

