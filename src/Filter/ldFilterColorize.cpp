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

#include "ldCore/Filter/ldFilterColorize.h"

#include <math.h>

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#include "ldCore/Helpers/Audio/ldHybridReactor.h"
#include "ldCore/Helpers/Audio/ldTempoAC.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Visualizer/ldLaserFilter.h"

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
    
    uint32_t c = ldColorUtil::colorHSV(hue*360, sat, value);
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
    int color = ldColorUtil::colorHSV(decColor1, 1.0, 1.0);

    color = ldColorUtil::colorHSV(cde, 1.0, 1.0);
    color = ldColorUtil::lerpInt(color, ldColorUtil::colorHSV(nde, 1.0, 1.0), clampf((y + 1.2)*0.6, 0, v));

    if (cde2 > 0) {
        if (fabs(-(y + 1)*0.5 + cde2) < 0.01)  color = ldColorUtil::colorHSV(ldMaths::periodIntervalKeeper(decColor1 + 80, 0, 360), 1.0, v);
    }
    if (cde3 > 0) {
        if (fabs(-(y + 1)*0.5 + cde3) < 0.06)  color = ldColorUtil::colorHSV(ldMaths::periodIntervalKeeper(decColor1 + 30, 0, 360), 1.0, v);
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



void FilterColorBlobs::process(Vertex &input) {

    LaserPoint t(input);

    float vv = t.r; if (t.g > vv) vv = t.g; if (t.b > vv) vv = t.b; if (vv == 0) return;

    float f = sqrtf(t.x*t.x + t.y*t.y);
    float a = atan2f(t.y, t.x)/6.28f;

    ldMusicManager* mm = ldCore::instance()->musicManager();

    const int nblobs = 48;
    const float scale = 6;
    float blobsize = 0.1f/2;
    if (m_type == 1) blobsize /= 1;
    if (m_type == 2) blobsize *= 2;
    static float rads[nblobs];
    static float angs[nblobs];
    static bool didinit = false;
    if (!didinit) {
        didinit = true;
        for (int i = 0; i < nblobs; i++) rads[i] = ((float)i)/nblobs * scale;
        //for (int i = 0; i < nblobs; i++) angs[i] = 1.0f/nblobs;
        for (int i = 0; i < nblobs; i++) angs[i] = (rand()%360)/360.0f;
    }

    float ta = 0;
    float tb = 0;
    float tc = 0;

    static float offset = 0;
    static int fcounter = 0;
    fcounter++;
    if (fcounter >= 100) {
        fcounter = 0;
        //offset += 0.01f;
        //if (offset > scale) offset -= scale;
    }
    offset += 0.00001f;
    if (m_type == 1) offset += 0.00003f;
    if (m_type == 2) offset += 0.00001f;

    if (offset > scale) offset -= scale;

    for (int i = 0; i < nblobs; i++) {
        float rad2 = rads[i] + offset;
        if (rad2 > scale) rad2 -= scale;
        if (rad2 > 2) continue;
        float df = rad2 - f;
        float da = angs[i] - a;
        if (da < 0) da++;
        if (da > 0.5f) da = 1-da;
        float dd = df*df+da*da*4;

        /*if (m_type == 1) {
            float xx = cosf(angs[i]*6.28f);
            float yy = sinf(angs[i]*6.28f);
            dd = (xx-t.x)*(xx-t.x)+(yy-t.y)*(yy-t.y);
        }*/
        if (m_type == 0) dd = sqrtf(dd)/8;

        dd /= blobsize;
        float ff = 1-dd;
        if (rad2 < 0.25f) ff *= rad2*4;
        if (ff < 0) ff = 0; else {
            if (m_type == 2) {
                if ((i%3) == 0) ta += ff;
                if ((i%3) == 1) tb += ff;
                if ((i%3) == 2) tc += ff;
            } else {
                if (i%2) tc += ff; else tb += ff;
            }
        }
    }
    if (m_type != 2) ta = 1;
    if (tb > 1) tb = 1;
    if (tc > 1) tc = 1;
    if (m_type == 0) {
        ta -= tb;
        ta -= tc;
        if (ta < 0) ta = 0;
    }
    if (m_type == 1) {
        ta = 0.5f;
    }

    t.r = ta*vv;
    t.g = tb*vv;
    t.b = tc*vv;

    if (m_type == 2) mm->hybridColorPalette->colorize(t.r, t.g, t.b);

    input = t.toVertex();

    if (m_type != 2)
    {
        float hh, ss;
        ldColorUtil::colorRGBtoHSVfloat(input.color[0], input.color[1], input.color[2], hh, ss, vv);
        hh += (offset/scale);
        hh -= (int) hh;
        ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, input.color[0], input.color[1], input.color[2]);
    }

}




void FilterColorFreq::process(Vertex &input) {

    LaserPoint t(input);

    float vv = t.r; if (t.g > vv) vv = t.g; if (t.b > vv) vv = t.b; if (vv == 0) return;

    float ta = 0;
    float tb = 0;
    float tc = 0;

    ldMusicManager* mm = ldCore::instance()->musicManager();

    float xx = (0.9f*t.x + 1)/2;
    float yy = (t.y + 1)/2;
    int index = (int) (clampf(mm->spectFrame.slsLen * xx, 0, mm->spectFrame.slsLen-1));
    float energy = mm->spectFrame.sls[index];
    energy = mm->spectAdvanced->value[index];
    //energy = mm->spectAdvanced2->value[index];
    energy = sqrtf(energy);
    energy = 1-energy;
    energy = energy*1.05f - 0.25f;
    energy = 1-energy;

    if (m_type != 0) {
        float f1 = mm->bass()*1.1f;
        float f2 = mm->mids()*0.9f;
        float f3 = mm->high()*0.7f;
        if (m_type == 2) {
            f1 = mm->mrSlowBass->output;
            //f2 = mm->mrFastBass->output;
            f2 = mm->mrSlowTreb->output;
            f3 = mm->mrFastTreb->output;
        }
        //f2 = (f1+f2)/2;
        float fa = 0;
        float fb = 1;
        float ft = 0.5f;
        float tx = xx;
        if (m_type >= 1) tx = fabsf(t.x);
        if (tx > 0.5f) {
            fa = f1;
            fb = f2;
            ft = (tx - 0.0f) / (0.5f);
        } else {
            fa = f2;
            fb = f3;
            ft = (tx - 0.5f) / (0.5f);
        }
        energy = fa*(1-ft) + fb*(ft-0);
        clampfp(energy, 0, 1);
        if (m_type <= 2) energy = energy * 0.7f + 0.15f;
    }

    ta = (yy < (energy+0.1f)) ? 1 : 0;
    tb = (yy > (energy-0.1f)) ? 1 : 0;
    tc = ta+tb-1;
    if (m_type == 2) {tb = 1; tc = ta*yy;}

    t.r = tb*vv;
    t.g = ta*vv;
    t.b = tc*vv;

    clampfp(t.r,0,1);clampfp(t.g,0,1);clampfp(t.b,0,1);

    if (m_type == 2) mm->hybridColorPalette->colorize(t.r, t.g, t.b);

    input = t.toVertex();

    float hm = 0;
    if (m_type == 1) hm = rcost(mm->mrSlowBass->spinOutput4) * 0.33f;
    //if (m_type == 2) mm->mrSlowBass->spinOutput4 + 0.25f*mm->mrSlowTreb->spinOutput4;
    if (m_type == 1) {
        float hh, ss;
        ldColorUtil::colorRGBtoHSVfloat(input.color[0], input.color[1], input.color[2], hh, ss, vv);
        hh += hm;
        hh -= (int) hh;
        ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, input.color[0], input.color[1], input.color[2]);
    }

}



