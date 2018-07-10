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

#include "ldFilterColorize.h"

#include <math.h>

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include <ldCore/Filter/ldColorUtils.h>
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#include "ldCore/Visualizations/util/AudioHelper/ldHybridReactor.h"
#include "ldCore/Visualizations/util/AudioHelper/ldTempoAC.h"
#include "ldCore/Visualizations/util/ColorHelper/ldColorUtil.h"
#include "ldCore/Visualizations/util/MathsHelper/ldGeometryUtil.h"
#include "ldCore/Visualizations/util/MathsHelper/ldMaths.h"
#include "ldCore/Visualizations/util/VisualizerHelper/ldLaserFilter.h"

ldFilterColorFade::ldFilterColorFade() {
    huebase = 0;
    huerange = 0.5;
    offset = 0;
    freq=0.5;
    y = false;
}

void ldFilterColorFade::process(Vertex &input) {
    
    LaserPoint t(input);
    float hue = 0;
    float sat = 1;
    float value = 1;
    
    float maxc = 0;
    maxc = MAX(maxc, t.r);
    maxc = MAX(maxc, t.g);
    maxc = MAX(maxc, t.b);
    value = maxc;
    
    float angle = freq*t.y + offset;
    if (y) angle = fabsf(freq*t.x) + offset;
    
    /*float r = cosf((angle+0.33)*M_PI*2)/2+0.5;
     float g = cosf((angle+0.67)*M_PI*2)/2+0.5;
     float b = cosf((angle)*M_PI*2)/2+0.5;*/
    
    hue = cosf((angle+0.67)*M_PI*2)/2+0.5;
    hue *= huerange;
    hue += huebase;
    hue -= (int)hue;
    
    uint32_t c = colorHSV(hue*360, sat, value);
    t.r = ((c>>16)&0xff)/255.0f;
    t.g = ((c>> 8)&0xff)/255.0f;
    t.b = ((c>> 0)&0xff)/255.0f;
    
    input = t.toVertex();


}

ldFilterColorLift::ldFilterColorLift() {
//    float cde = 0.5f;// currentData[0].effect;
//    float cde2 = 0.65f;// currentData[0].effect2;
//    float cde3 = 0.75f;// currentData[0].effect3;
//    float nde = 0.25f;// nextData[0].effect
}

void ldFilterColorLift::process(Vertex &input) {
    bool isBlank = input.color[0] == 0 &&  input.color[1] == 0 && input.color[2] == 0;
    if(isBlank) {
        return;
    }

    LaserPoint t(input);
    float y = t.y;
    float v = MAX(t.r, MAX(t.g, t.b));
    clampfp(v, 0, 1);
    ldMusicManager* mm = ldCore::instance()->musicManager();

    if (true) { // nyi
        cde = mm->musicFeature1->statMoodFunky * 250;//m_musicManager->mrSlowTreb->spinOutput4 * 180;
        cde2 = mm->hybridAnima->outputTrackPosition;
        cde3 = 1 - mm->tempoACFast->phaseSmooth;
        //colorFilter3.nde = m_musicManager->musicFeature1->statMoodMelodic * 250;// m_musicManager->mrSlowTreb->spinOutput4*250;
        nde = mm->mrSlowTreb->spinOutput4 * 250;
    }

    float decColor1 = cde;
    int color = colorHSV(decColor1, 1.0, 1.0);

    color = colorHSV(cde, 1.0, 1.0);
    color = ldColorUtil::lerpInt(color, colorHSV(nde, 1.0, 1.0), clampf((y + 1.2)*0.6, 0, v));

    if (cde2 > 0) {
        if (fabs(-(y + 1)*0.5 + cde2) < 0.01)  color = colorHSV(ldMaths::periodIntervalKeeper(decColor1 + 80, 0, 360), 1.0, v);
    }
    if (cde3 > 0) {
        if (fabs(-(y + 1)*0.5 + cde3) < 0.06)  color = colorHSV(ldMaths::periodIntervalKeeper(decColor1 + 30, 0, 360), 1.0, v);
    }
    
    t.r = ((0x00ff0000 & color) >> 16) / 255.0f;
    t.g = ((0x0000ff00 & color) >>  8) / 255.0f;
    t.b = ((0x000000ff & color) >>  0) / 255.0f;

    input = t.toVertex();
}

void ldFilterColorDrop::process(Vertex &input) {

	LaserPoint t(input);
	float f = sqrtf(t.x*t.x + t.y*t.y);

	ldMusicManager* mm = ldCore::instance()->musicManager();

	static bool was = false;
	static bool wait = false;
	static int old = 0;
	static int nw = 1;
	float v = mm->tempoACFaster->phaseSmooth;
	bool iz = (v > 0.95);
	if (iz && !was) {
		wait = !wait;
		if (!wait) {
			old = nw;
			nw = (nw + 1) % 4;
		}
	}
	was = iz;

	float b = MAX(MAX(t.r, t.g), t.b);

	if (wait) v = 0;
	bool flop = (f > (1 - v));
	int ix = flop ? old : nw;
	t.r = ((0 == (ix % 4))) ? b : 0;
	t.g = ((1 == (ix % 2))) ? b : 0;
	t.b = ((2 == (ix % 4))) ? b : 0;
	mm->hybridColorPalette->colorize(t.r, t.g, t.b);

	//t.c *= c;
	//t.c *= c;

	input = t.toVertex();
}
