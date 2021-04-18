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

#include <cmath>

#include <QtCore/QDebug>

#include "ldCore/ldCore.h"
#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"
#include "ldCore/Helpers/Audio/ldHybridReactor.h"
#include "ldCore/Helpers/Audio/ldTempoAC.h"
#include "ldCore/Helpers/Color/ldColorUtil.h"
#include "ldCore/Helpers/Maths/ldGeometryUtil.h"
#include "ldCore/Helpers/Maths/ldMaths.h"
#include "ldCore/Helpers/Visualizer/ldLaserFilter.h"

ldFilterColorFade::ldFilterColorFade(bool isProcessMusic)
    : ldFilter()
    , m_colors(1)
    , m_min_colors(1)
    , m_max_colors(3)
    , m_isProcessMusic(isProcessMusic)
{
}

void ldFilterColorFade::process(ldVertex &input) {
    if(m_isProcessMusic) {
        ldMusicManager *m = ldCore::instance()->musicManager();
        offset = 1 - m->hybridAnima->outputTrackPosition;
        huebase = m->hybridAutoColor2->selectorColorHue1.indexFloat / 4.0f + 0.17f;
        huerange = (m->hybridAutoColor2->selectorColorHue2.indexFloat / 5.0f + 0.33f) / 4.0f / 4.0f * m_colors;
        freq = 0.33f;
    }

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

void ldFilterColorLift::process(ldVertex &input) {
    if(input.isBlank()) {
        return;
    }

    LaserPoint t(input);
    float y = t.y;
    float v = MAX(t.r, MAX(t.g, t.b));
    clampfp(v, 0, 1);
    ldMusicManager* mm = ldCore::instance()->musicManager();

//    if (true) { // nyi
        float cde = mm->musicFeature1->statMoodFunky * 250;//m_musicManager->mrSlowTreb->spinOutput4 * 180;
        float cde2 = mm->hybridAnima->outputTrackPosition;
        float cde3 = 1 - mm->tempoACFast->phaseSmooth;
        //colorFilter3.nde = m_musicManager->musicFeature1->statMoodMelodic * 250;// m_musicManager->mrSlowTreb->spinOutput4*250;
        float nde = mm->mrSlowTreb->spinOutput4 * 250;
//    }

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

void ldFilterColorDrop::process(ldVertex &input) {

	LaserPoint t(input);
	float f = sqrtf(t.x*t.x + t.y*t.y);

	ldMusicManager* mm = ldCore::instance()->musicManager();

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



FilterColorBlobs::FilterColorBlobs(int type)
    : m_type(type)
{
    for (int i = 0; i < nblobs; i++)
        rads[i] = ((float)i)/nblobs * scale;
    //for (int i = 0; i < nblobs; i++) angs[i] = 1.0f/nblobs;
    for (int i = 0; i < nblobs; i++)
        angs[i] = (rand()%360)/360.0f;
}

void FilterColorBlobs::process(ldVertex &input) {

    LaserPoint t(input);

    float vv = t.r; if (t.g > vv) vv = t.g; if (t.b > vv) vv = t.b; if (vv == 0) return;

    float f = sqrtf(t.x*t.x + t.y*t.y);
    float a = atan2f(t.y, t.x)/6.28f;


    float blobsize = 0.1f/2;
    if (m_type == 1) blobsize /= 1;
    if (m_type == 2) blobsize *= 2;

    float ta = 0;
    float tb = 0;
    float tc = 0;

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

    if (m_type == 2) {
        ldMusicManager* mm = ldCore::instance()->musicManager();
        mm->hybridColorPalette->colorize(t.r, t.g, t.b);
    }

    input = t.toVertex();

    if (m_type != 2)
    {
        float hh, ss;
        ldColorUtil::colorRGBtoHSVfloat(input.r(), input.g(), input.b(), hh, ss, vv);
        hh += (offset/scale);
        hh -= (int) hh;
        ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, input.r(), input.g(), input.b());
    }

}

QString FilterColorBlobs::name()
{
    if (m_type == 0) return QObject::tr("Color Blobs A");
    else if (m_type == 1) return QObject::tr("Color Blobs B");
    else return QObject::tr("Color Blobs C");
}

bool FilterColorBlobs::isMusicAware() const
{
    return (m_type == 2);
}



void FilterColorFreq::process(ldVertex &input) {

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

    if (m_type == 1) {
        float hm = rcost(mm->mrSlowBass->spinOutput4) * 0.33f;
        float hh, ss;
        ldColorUtil::colorRGBtoHSVfloat(input.r(), input.g(), input.b(), hh, ss, vv);
        hh += hm;
        hh -= (int) hh;
        ldColorUtil::colorHSVtoRGBfloat(hh, ss, vv, input.r(), input.g(), input.b());
    }
}

// ---------- ldShimmerFilter ----------

ldShimmerFilter::ldShimmerFilter()
    : ldFilter()
    , m_colors(1)
    , m_min_colors(0.5f)
    , m_max_colors(2.0f)
{
}

void ldShimmerFilter::process(ldVertex &input) {
    float hueoffset = 0;
    ldMusicManager* m = ldCore::instance()->musicManager();
    //hueoffset = 1.0f/3.0f * m->tempoACFaster->phaseSmooth;
    float fps = 1.0f/AUDIO_UPDATE_DELTA_S * 1.0f/30000.0f;
    m_counter += m->tempoACFaster->freqSmooth*fps*512;
//    bool reset = (((int) counter) != 0);
    m_counter -= (int) m_counter;
    float fc = m_counter;

    float str = 1.0f/3.0f * 0.50f;
    //str *= clampf(0.0f+1.0f*m->tempoACSlow->phaseSmooth*4-3, 0, 1);
    hueoffset = 1 + sinf(fc*M_PI*2) * str * m_colors;
    ldColorUtil::hueShift(input.b(), input.g(), input.b(), hueoffset);
}




// util
static float cycle01(float f) {return f - floorf(f);}
//static float mirror01(float f) {if (int(floorf(f)) % 2) f = -f; return cycle01(f);}
static float clamp01(float f) {return clampf(f, 0, 1);}
static void clamp01p(float &f) {f = clampf(f, 0, 1);}
static float rand01() {return (float(rand()))/RAND_MAX;}
static int randi(int n) {return rand()%n;}


FilterColorScroll::FilterColorScroll(int type) {m_type = type;}
void FilterColorScroll::process(ldVertex &input) {

    LaserPoint t(input);
    ldMusicManager* mm = ldCore::instance()->musicManager();
    float gfreq = 1.0f/30000.0f;
    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);
    float oldv = v, olds = s;

    float spd = 1;//mm->mrSlowBass->output;
    float spd2 = 1;//mm->mrSlowBass->output;
    bool horz = 1;
    bool flat = 1;
    bool reflex = 1;
    bool asymm = 1;
    bool left = 1;
    bool side = 1;
    float wide = 3.0f;
    float walk = pulse1.value;//mm->mrSlowBass->walkerOutput;
    float beat = pulse2.value;//mm->mrSlowTreb->output;
    bool square = 1;
    bool fixedhue = 1;
    float huespd = 1;
    if (m_type == 2) {
        spd = 1;//0.125f*mm->tempoTrackerFast()->bpm()/60.0f;//mm->mrSlowBass->output;mm->mrSlowBass->output*mm->mrSlowBass->output*0.875f+0.125f;
        //spd2 = 32*mm->tempoTrackerSlow()->bpm()/60.0f;//mm->mrSlowBass->output;
        spd2 = 4;//mm->mrSlowBass->output;
        horz = 1;
        flat = 0;
        reflex = 1;
        asymm = 1;
        left = 1;
        side = 1;
        wide = 1.0f;
        walk = pulse2.value;//mm->mrSlowBass->walkerOutput;//pulse2.value;//mm->mrSlowBass->walkerOutput;//pulse1.value;
        beat = mm->mrSlowTreb->output;//pulse2.value;
        square = 1;
        fixedhue = 0;
        huespd = (1.0f / 24)*clampf(mm->mrSlowTreb->output*mm->mrSlowTreb->output*0.875f+0.125f,0,1);
    } else if (m_type == 1) {
        spd = mm->mrSlowBass->output*mm->mrSlowBass->output*0.875f+0.125f;
//        float spd2 = 1;//mm->mrSlowBass->output;
        horz = 0;
        flat = 0;
        reflex = 0;
        asymm = 0;
        left = 0;
        side = 0;
        wide = 2.0f;
        walk = mm->mrSlowBass->walkerOutput;//pulse1.value;
        beat = mm->mrSlowTreb->output;//pulse2.value;
        square = 0;
        fixedhue = 0;
        //huespd = 1.0f / 6;
        huespd = (1.0f / 6);
    }

    spd = clampf(spd * 2, 0.5, 1); spd *= spd;
    pulse1.freq = spd / 2;
    pulse2.freq = spd2 / 32;
    pulse3.freq = huespd;
    pulse1.update(gfreq);
    pulse2.update(gfreq);
    pulse3.update(gfreq);
    float xscroll = pulse1.value;
    float cscroll = pulse2.value;
    float hscroll = pulse3.value;

    float tv = MAX(MAX(t.r, t.g), t.b);
    float tx = horz?t.x:t.y; if (!left) tx = -tx;
    float ty = horz?t.y:t.x;

    float curve = sinf(walk*2*M_PIf)*1.0f;
    if (!reflex) curve = rcost(walk)*1.0f;
    float dy = flat?0:1-cosf(fabsf(ty)*M_PI_2f*fabsf(curve));
    if (reflex) if (curve > 0) dy *= -1;
    if (asymm) if (ty > 0) dy *= -1;
    if (!side) dy *= -1;
    float dx = rcost(tx / wide + xscroll + dy / wide);

    if (square) dx *= dx;

    s = 1;
    v = tv;
    h = dx*0.2f + 0.4f;
    s = 1-dx*dx*0.5f;
    s = 1-dx*dx*beat;
    h += rcost(cscroll)*0.4f-0.2f;
    if (!fixedhue) h += hscroll;
    h += 1; h -= int(h);

    if (m_type == 3) {
        ldMusicManager& m = *ldCore::instance()->musicManager();
        float bps = ldFilterMath::normalizeToBPM(25, m.bestBpm()) / 60.0f;
        if(cmpf(bps, 0)) bps = 1/60.f;
        //if (fp.tick || (fp.value < (1.0f/16.0f))) fp.freq = bps/32;
        pulse4.freq = bps/32;
        pulse4.update(1.0f/30000.0f);

        float x = input.x(), y = input.y();
        float wfreq = 0.50f;
        float wangle = -pulse4.value*16;//rcost(pulse4.value*2)*3;
        float rad = 2.6f;
        float angle = pulse4.value*3;
        float ripple = ldFilterMath::rippleEffect(x, y, wfreq, wangle, rad, angle);
        //ripple2 = clamp01(ripple2*ripple2*3-2.0f)*0.75f;
        //ripple2 = clamp01(ripple2*ripple2*2-1.0f)*0.50f;
        ripple = clamp01(ripple*ripple*2-1.0f)*0.66f;
        
        h = ripple;
        h = cycle01(ripple + pulse4.value*9);
        s = clamp01(olds + 1 - ripple*2/0.66f);
        v = oldv;// * ripple;

        ldColorUtil::hsv2rgb(h, s, v, input.r(), input.g(), input.b());
        return;

    }

    input = t.toVertex();
    ldColorUtil::hsv2rgb(h, s, v, input.r(), input.g(), input.b());
    return;

}

QString FilterColorScroll::name()
{
    if (m_type == 2)
        return QObject::tr("Colorize Scroll C");

    if (m_type == 1)
        return QObject::tr("Colorize Scroll B");

    return QObject::tr("Colorize Scroll A");
}


FilterColorRainbeam::FilterColorRainbeam(int type) {m_type = type;}
void FilterColorRainbeam::process(ldVertex &input) {

    //    float freq = 1.0f/30000.0f;
    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);
    float oldv = v, olds = s;

    ldMusicManager& m = *ldCore::instance()->musicManager();
    float bps = ldFilterMath::normalizeToBPM(25, m.bestBpm()) / 60.0f;
    if(cmpf(bps, 0)) bps = 1/60.f;
    //if (fp.tick || (fp.value < (1.0f/16.0f))) fp.freq = bps/32;
    fp.freq = bps/32;
    fp.update(1.0f/30000.0f);

    float x = input.x(), y = input.y();
    float wfreq = 0.50f;
    float wangle = -fp.value*16;//rcost(pulse4.value*2)*3;
    float rad = 2.6f;
    float angle = fp.value*3;
    float ripple = ldFilterMath::rippleEffect(x, y, wfreq, wangle, rad, angle);
    float ripple2 = clamp01(ripple*ripple*1.5f-0.75f)*0.66f;

    h = ripple;
    h = cycle01(ripple2 + fp.value*6);
    s = clamp01(olds - 1 + ripple*2);
    float vf = 1;
    //vf = rcostc(fp.value*4);
    //vf = (1-vf)*1 + (0+vf)*ripple;
    v = oldv * vf;

    ldColorUtil::hsv2rgb(h, s, v, input.r(), input.g(), input.b());
    return;

}


ColorPanelCanvas::ColorPanelCanvas() {init();}
void ColorPanelCanvas::init() {
        xx = int(clampf(xx, 0, maxsize-1));
        yy = int(clampf(yy, 0, maxsize-1));
        for (int x = 0; x < xx; x++) {
            for (int y = 0; y < yy; y++) {
                array[x][y] = 0;
            }
        }
}
bool ColorPanelCanvas::isint(float f) {return (f <= float(int(f)));}
void ColorPanelCanvas::getRandXYModIntCross(int& xm, int& ym) {
    xm = ym = 0;
    int r = randi(4);        if (r == 0) xm = 1;        if (r == 1) xm = -1;        if (r == 2) ym = 1;        if (r == 3) ym = -1;
}
void ColorPanelCanvas::getRandXYModIntRound(int& xm, int& ym) {
    xm = ym = 0;
    int r = randi(4);        if (r == 0) xm = 1;        if (r == 1) xm = -1;        if (r == 2) ym = 1;        if (r == 3) ym = -1;
    r = randi(4);            if (r == 0) xm = 1;        if (r == 1) xm = -1;        if (r == 2) ym = 1;        if (r == 3) ym = -1;
}

void ColorPanelCanvas::grow(float delta) {
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
void ColorPanelCanvas::spread(float delta) {
    int times = int(delta*xx*yy);
    if (times < 1) times = 1;
    for (int i = 0; i < times; i++) spread();
}
void ColorPanelCanvas::spread() {
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
void ColorPanelCanvas::drop() {
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
float ColorPanelCanvas::getf(float x, float y) {
    clampfp(x, 0, 1);
    clampfp(y, 0, 1);
    int ix = int(clampf(floorf(x*xx), 0, xx-1));
    int iy = int(clampf(floorf(y*yy), 0, yy-1));
    float f = clampf(array[ix][iy] / steps, 0, 1);
    return f;
}
float ColorPanelCanvas::getfSmooth(float x, float y) {
    if (x < 0) x += int(x) + 2;
    if (y < 0) y += int(y) + 2;
    x -= float(int(x));
    y -= float(int(y));
    float x2 = x*xx-0.5f+xx;
    float y2 = y*yy-0.5f+yy;
    int ix = (int)x2;
    int iy = (int)y2;
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


FilterColorPanels::FilterColorPanels(bool alt) {m_alt = alt;}
void FilterColorPanels::process(ldVertex &input) {

    LaserPoint t(input);
    ldMusicManager* mm = ldCore::instance()->musicManager();

    pulse1.gatemax = 3;
    pulse2.gatemax = 3;
    pulse1.gate = mm->mrSlowBass->walkerClickOutput;// > 15/16.0f;
    pulse2.gate = 1;

    float dropspeed = 1.0f;
    float calcspeed = 120;
    float growspeed = 0.02f*2;
    float spreadspeed = 0.05f*2;
    panel.xx = 5;
    panel.yy = 5;
    panel.steps = 5;
    panel.instant = false;
    panel.followsteps = true;
    panel.thresh = 0.5f;
    panel.round = false;

    if (m_alt) {
        pulse1.gate = 1;
        pulse2.gate = 1;
        dropspeed = 2.0f;
        calcspeed = 120;
        growspeed = 0.02f*1.5f;
        spreadspeed = 0.15f;
        panel.xx = 12;
        panel.yy = 12;
        panel.steps = 9;
        panel.instant = false;
        panel.followsteps = 0;
        panel.thresh = 0.25f;
        panel.round = true;
    }

    float gfreq = 1.0f/30000.0f;
    pulse1.freq = dropspeed;
    pulse2.freq = calcspeed;
    pulse1.update(gfreq);
    pulse2.update(gfreq);


    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);

    if (pulse1.tick) {
        panel.drop();
    }

    if (pulse2.tick) {
        if (!panel.instant) panel.grow(growspeed);
        panel.spread(spreadspeed);
    }

    {
        float f = panel.getf((t.x+1)/2, (t.y+1)/2);
        //f = panel.getfSmooth((t.x+1)/2, (t.y+1)/2);
        if (m_alt) f = panel.getfSmooth((t.x*(1-1.5f/panel.xx)+1)/2, (t.y*(1-1.5f/panel.xx)+1)/2);
        if (m_alt) f = (f*2)-floorf(f*2);
        //else f = (f*4)-floorf(f*4);
        h = f;
        s = 1;
        //v = tv;
        input = t.toVertex();
        ldColorUtil::hsv2rgb(h, s, v, input.r(), input.g(), input.b());
        return;
    }

}

FilterColorGlass::FilterColorGlass(bool alt) {m_alt = alt;}
void FilterColorGlass::process(ldVertex &input) {

    LaserPoint t(input);
    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);
    ldMusicManager* mm = ldCore::instance()->musicManager();

    float gfreq = 1.0f/30000.0f;
    pulse1.freq = 0.5f;
    pulse1.gate = mm->mrSlowBass->walkerClickOutput;
    pulse1.gatemax = 4;
    pulse1.update(gfreq);
    if (pulse1.tick) {curoffset += 1*0.272f*(0.5f+0.5f*rand01()); curoffset -= int(curoffset);}

    float am = 2;
    bool useangle = true;
    bool usepos = false;
    bool reangle = true;
    bool colorcycle = false; //0*true;
    bool halfspec = false;

    if (m_alt) {
        //am = 2;
        useangle = false;
        usepos = true;
        reangle = false;
        colorcycle = true;
        halfspec = true;
    }


    float minv = 0.02f;
    bool ison = (v > minv);

    float dx = t.x - ox;
    float dy = t.y - oy;
    ox = t.x;
    oy = t.y;

    float dist2 = dx*dx+dy*dy;
    float mindist = 0.001f;
    float mindist2 = mindist*mindist;
    bool ismoving = (dist2 >= mindist2);

    float toangle = oangle;
    float tangle = atan2(dy, dx) / (M_PIf*2);
    if (ison && ismoving) oangle = tangle;

    float tpos = 0;
    float posx = 1;
    tpos += rcost(t.x * posx);
    tpos += rcost(t.y * posx);
    tpos -= floorf(tpos);
    float adist = MIN(MIN(fabsf(toangle - oangle + 1), fabsf(toangle - oangle - 1)), fabsf(toangle - oangle));
    if (reangle) {
        if (ismoving) {
            if (adist > 1.0f / 12.0f) {
                curvalue = (useangle?tangle*am:0) + (usepos?tpos:0);
            }
        }
    }

    if (phase == 0) {
        if (ismoving && ison) {
            phase = 1;
            curvalue = (useangle?tangle*am:0) + (usepos?tpos:0);
        }
    } else if (phase == 1) {
        if (!ismoving || !ison) phase = 0;
    }

    if (phase == 0) {
        s = 0;
        v = 0;
    } else if (phase == 1) {
        h = curvalue;
        if (halfspec) h = rcost(curvalue)/2;
        if (colorcycle) h += curoffset;
        h -= (int) h;
        s = 1;
    }

    //input = t.toVertex();
    ldColorUtil::hsv2rgb(h, s, v, input.r(), input.g(), input.b());
    return;
}


GridNoise::GridNoise() {
    for (int x = 0; x < msize; x++) {
        for (int y = 0; y < msize; y++) {
            a[x][y] = 0;
        }
    }
}
void GridNoise::init(int s) {
    if (s > msize) s = msize;
    if (s < 2) s = 2;
    size = s;
    clear(0);
}
void GridNoise::clear(float z) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            a[x][y] = z;
        }
    }
}
void GridNoise::noise(float f) {
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            a[x][y] += rand01() * f;
            clampfp(a[x][y], 0, 1);
        }
    }
}
void GridNoise::stripe(float z, bool h, bool v) {
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
void GridNoise::plasma(float d, float f) {
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
float GridNoise::get(float x, float y, float zoom, float rot) {
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

float GridNoise::get(float x, float y) {
    if (x < 0) x += int(x) + 2;
    if (y < 0) y += int(y) + 2;
    x -= float(int(x));
    y -= float(int(y));
    float x2 = x*size-0.5f+size;
    float y2 = y*size-0.5f+size;
    int ix = (int)x2;
    int iy = (int)y2;
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

void ColorMap::getRGBClamp(float f, float& r, float& g, float& b) {
    clampfp(f, 0, 1);
    getRGB(f, r, g, b);
}
void ColorMap::getRGBCycle(float f, float& r, float& g, float& b) {
    f -= floorf(f);
    clampfp(f, 0, 1);
    getRGB(f, r, g, b);
}
void ColorMap::getRGBMirror(float f, float& r, float& g, float& b) {
    if (int(floorf(f)) % 2) f = -f;
    f -= floorf(f);
    clampfp(f, 0, 1);
    getRGB(f, r, g, b);
}


void ColorMapHSVGradient::getRGB(float f, float& r, float& g, float& b) {
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

void ColorMapBPSW::getRGB(float f, float& r, float& g, float& b) {
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

// more filters

FilterColorAura::FilterColorAura(bool alt) {
    m_alt = alt;
    float fx = 0.125f*0.25f; if (m_alt) fx /= 2;
    float fy = 2*0.25f / 16.0f;
    fc1.start(fx/30000.0f, 0.0f);
    fc2.start(fx/30000.0f, 0.5f);
    fc3.start(fx/30000.0f, 0.0f);
    fc4.start(fx/30000.0f, 0.5f);
    fc5.start(fy/30000.0f, 0.5f);
    fc6.start(0.5f/30000.0f, 0.5f);
}

void FilterColorAura::process(ldVertex &input) {

    ldMusicManager* mm = ldCore::instance()->musicManager();

    fc6.gate = mm->mrFastBass->walkerClickOutput;
    fc1.update(1);    fc2.update(1);    fc3.update(1);
    fc4.update(1);    fc5.update(1);    fc6.update(1);

    float p1 = 1.5f;
    float p2 = 0.15f;
    float n1 = 0.25f;
    int size = 16;
    if (fc1.tick) {        sbg1.init(size);        sbg1.noise(n1);        sbg1.plasma(p1, p2);        sbg1.plasma(p1, p2);    }
    if (fc2.tick) {        sbg2.init(size);        sbg2.noise(n1);        sbg2.plasma(p1, p2);        sbg2.plasma(p1, p2);    }
    if (fc3.tick) {        sbg3.init(size);        sbg3.noise(n1);        sbg3.plasma(p1, p2);        sbg3.plasma(p1, p2);    }
    if (fc4.tick) {        sbg4.init(size);        sbg4.noise(n1);        sbg4.plasma(p1, p2);        sbg4.plasma(p1, p2);    }

    LaserPoint t(input);
    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);

    float mag = 0;

    float xx = (t.x+1)/2;
    float yy = (t.y+1)/2;

    float cf1 = fc1.value;
    float cf2 = fc2.value;
    float cf3 = fc3.value;
    float cf4 = fc4.value;
    float f1 = rcostc(cf1);
    float f2 = rcostc(cf2);
    float f3 = rcostc(cf3);
    float f4 = rcostc(cf4);

    float ff = 0;
    float rot = 2.5f*1;
    float zoom1 = 1*1.0f;
    float zoom2 = 0*1.5f;
    ff += f1*sbg1.get(xx,yy,zoom1+zoom2*cf1,0+rot*cf1*1);
    ff += f2*sbg2.get(xx,yy,zoom1+zoom2*cf2,0+rot*cf2*1+0.125f);
    ff += f3*sbg3.get(xx,yy,zoom1+zoom2*cf3,0-rot*cf3*1+0.125f);
    ff += f4*sbg4.get(xx,yy,zoom1+zoom2*cf4,0-rot*cf4*1);
    ff = clampf(ff/2, 0, 1);
    float fmag = ff;
    mag += fmag;

    float r = sqrtf(t.x*t.x+t.y*t.y);
    float rmag = clampf((1-r)*2-0.75f,m_alt?0:-0.5f,1);
    mag += rmag;

    clamp01p(mag);
    mag = clampf(mag*1.5f-0.25f,0,1);
    clamp01p(mag);

    ColorMapBPSW cmap;

    cmap.v = 1;
    cmap.getRGB(mag*v, input.r(), input.g(), input.b());
    cmap.v = v;

    if (!m_alt) {
        cmap.hue6 = 5;
        cmap.v = v;
        float mag2 = clampf(mag*0.7f+0.3f, 0.3f, 1);
        cmap.getRGB(mag2, input.r(), input.g(), input.b());
    } else {
        if (fc6.tick) huecount = (huecount + 1) % 6;
        cmap.hue6 = huecount;
        cmap.v = 1;
        float mag2 = clampf(mag*0.66f, 0.0f, 0.66f);
        cmap.getRGB(mag2*v, input.r(), input.g(), input.b());
    }
    return;

}

FilterColorAcid::FilterColorAcid(bool alt) {
    m_alt = alt;
    float fx = 4.0f / 4 / 4 * (!m_alt?0.5f:1.5f);
    fc1.start(fx/30000.0f, 0.00f);
    fc2.start(fx/30000.0f, 0.50f);
    fc3.start(fx/30000.0f, 0.25f);
    fc4.start(fx/30000.0f, 0.75f);
    float fy = 1.0f / 3 / 32;
    fc5.start(fy/30000.0f, 0.5f);
    fc6.start(0.66f/30000.0f, 0.5f);
}
void FilterColorAcid::process(ldVertex &input) {

    ldMusicManager* mm = ldCore::instance()->musicManager();

    fc6.gate = mm->mrSlowTreb->walkerClickOutput;
    fc6.gatemax = 16;
    fc1.update(1);    fc2.update(1);    fc3.update(1);
    fc4.update(1);    fc5.update(1);    fc6.update(1);

    int sa = !m_alt?16:7;
    float p1 = 2.5f;
    float p2 = 0.1f;
    if (fc1.tick) {sbg1.init(sa); sbg1.plasma(p1, p2);}
    if (fc2.tick) {sbg2.init(sa); sbg2.plasma(p1, p2);}
    if (fc3.tick) {sbg3.init(sa); sbg3.plasma(p1, p2);}
    if (fc4.tick) {sbg4.init(sa); sbg4.plasma(p1, p2);}

    LaserPoint t(input);
    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);
    float xx = (t.x+1)/2;
    float yy = (t.y+1)/2;

    float cf1 = fc1.value;
    float cf2 = fc2.value;
    float cf3 = fc3.value;
    float cf4 = fc4.value;
    float f1 = rcostc(cf1);
    float f2 = rcostc(cf2);
    float f3 = rcostc(cf3);
    float f4 = rcostc(cf4);

    float ff = 0;
    float aa = 1.5f;
    float bb = -1.0f;
    float dd = 0;
    if (m_alt) {
        aa = 0.33f;
        bb = 1.33f;
        dd = 0.33f;
    }
    ff += f1*sbg1.get(xx,yy,aa+bb*cf1,dd*cf1);
    ff += f2*sbg2.get(xx,yy,aa+bb*cf2,dd*cf2);
    ff += f3*sbg3.get(xx,yy,aa+bb*cf3,-dd*cf3);
    ff += f4*sbg4.get(xx,yy,aa+bb*cf4,-dd*cf4);

    ff = clampf(ff/1, 0, 1);
    clampfp(ff,0,1);

    float r = sqrtf(t.x*t.x+t.y*t.y);// / sqrtf(2);
    float rr = clampf(r*1.75f-0.125f,0,1.0f);

    float br = (1-rr)+ff;
    float b2 = (1-rr)*(ff+0.0f);

    clampfp(br,0,1);
    clampfp(b2,0,1);

    input = t.toVertex();
    h = clampf(ff*2-0.5f,0,1)*0.33f + fc5.value;  h -= floorf(h);
    h = clampf(ff*1.5f-0.25f,0,1);
    h *= h;
    h *= 0.66f;
    h += 0.66f;
    h -= floorf(h);
    s = 1;
    ldColorUtil::hsv2rgb(h, s, v, input.r(), input.g(), input.b());

    ColorMapHSVGradient cg;
    cg.h2 = 1;//0.6667f;
    cg.s1 = 1;
    cg.v1 = v;
    cg.h1 = 0;//0.9999f;
    cg.s2 = 1;
    cg.v2 = v;
    cg.getRGB(clampf(ff*1.5f-0.25f,0,1)*1, input.r(), input.g(), input.b());

    if (m_alt) {
        ColorMapBPSW cm;
        if (fc6.tick) huecount = (huecount + 1) % 6;
        cm.hue6 = huecount;
        cm.v = 1;
        cm.getRGB(clampf(ff*1.5f-0.25f,0.15f,1)*v, input.r(), input.g(), input.b());
    }
    return;
}

static void lavaEffect(float xx, float yy, float c, float& x2, float& y2) {
    float z = 5;
    float yscale = 1+c*(z-1);
    float xscale = z/yscale;
    x2 = (xx/xscale+1)/2;
    y2 = (yy+1)/2/yscale;
}
FilterColorLava::FilterColorLava(bool alt) {
    m_alt = alt;
    float fx = 0.5f*0.5f*(!m_alt?0.25f:0.66f);
//    float fy = 2*0.25f / 16.0f;
    fc1.start(fx/30000.0f, 0.0f);
    fc2.start(fx/30000.0f, 0.5f);
    fc3.start(fx/30000.0f, 0.25f);
    fc4.start(fx/30000.0f, 0.75f);
//    fc5.start(fy/30000.0f, 0.5f);
//    fc6.start(0.5f/30000.0f, 0.5f);
}
void FilterColorLava::process(ldVertex &input) {
//    ldMusicManager* mm = ldCore::instance()->musicManager();

//    fc6.gate = mm->mrFastBass->walkerClickOutput;
    fc1.update(1);    fc2.update(1);
    fc3.update(1);    fc4.update(1);
//    fc5.update(1);    fc6.update(1);

    float p1 = 1.5f;
    float p2 = 0.1f;
    float n1 = 0.2f;
    int size = 16;
    //if (!m_alt) size = 8;
    if (fc1.tick) {        sbg1.init(size);        sbg1.noise(n1);        sbg1.plasma(p1, p2);        sbg1.plasma(p1, p2);    }
    if (fc2.tick) {        sbg2.init(size);        sbg2.noise(n1);        sbg2.plasma(p1, p2);        sbg2.plasma(p1, p2);    }
    if (fc3.tick) {        sbg3.init(size);        sbg3.noise(n1);        sbg3.plasma(p1, p2);        sbg3.plasma(p1, p2);    }
    if (fc4.tick) {        sbg4.init(size);        sbg4.noise(n1);        sbg4.plasma(p1, p2);        sbg4.plasma(p1, p2);    }

    LaserPoint t(input);    if (m_alt) {t.y *= -1;}
    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);


    float mag = 0;

//    float xx = (t.x+1)/2;
    float yy = (t.y+1)/2;

    float cf1 = fc1.value;
    float cf2 = fc2.value;
    float cf3 = fc3.value;
    float cf4 = fc4.value;
    float f1 = rcostc(cf1);
    float f2 = rcostc(cf2);
    float f3 = rcostc(cf3);
    float f4 = rcostc(cf4);

    float ff = 0;
    float z = !m_alt?1:1.5f;
    float xx1, yy1; lavaEffect(t.x,t.y,cf1,xx1,yy1);if (m_alt) yy1 = cycle01(yy1-cf1*2);
    float xx2, yy2; lavaEffect(t.x,t.y,cf2,xx2,yy2);if (m_alt) yy2 = cycle01(yy2-cf2*2);
    float xx3, yy3; lavaEffect(t.x,t.y,cf3,xx3,yy3);if (m_alt) yy3 = cycle01(yy3-cf3*5);
    float xx4, yy4; lavaEffect(t.x,t.y,cf4,xx4,yy4);if (m_alt) yy4 = cycle01(yy4-cf4*5);
    ff += f1*sbg1.get(xx1,yy1,z,0*0.125f);
    ff += f2*sbg2.get(xx2,yy2,z,0*0.125f);
    ff += f3*sbg3.get(xx3,yy3,z,1*0.125f);
    ff += f4*sbg4.get(xx4,yy4,z,1*0.125f);
    ff = clampf(ff/2.0f, 0, 1);
    ff = clampf(ff*1.6f-0.3f, 0, 1);
    float fmag = ff;
    mag += fmag;

    float chmag = clamp01(1-t.x*t.x);
    mag *= chmag * 1.0f;// - 0.25f;
    float bmag = clamp01(1-yy*1.25f);
    mag += bmag * 0.5f;

    clamp01p(mag);
    if (!m_alt) mag = clampf(mag*0.75f+0.00f,0.00,0.65f);
    if (m_alt) mag = clampf(mag*mag*0.85f+0.15f,0.25f,1);
    clamp01p(mag);

    ColorMapBPSW cg;
    cg.hue6 = !m_alt?0:1;
    cg.v = !m_alt?1:v;
    mag *= !m_alt?v:1;
    cg.getRGB(mag, input.r(), input.g(), input.b());
    return;

}

FilterColorVolt::FilterColorVolt(bool alt) {m_alt = alt;}
void FilterColorVolt::process(ldVertex &input) {
    ldMusicManager* mm = ldCore::instance()->musicManager();
    float h, s, v;
    ldColorUtil::rgb2hsv(input.r(), input.g(), input.b(), h, s, v);
    if (!m_alt) {
        if (v < 0.01f) return;
        if (s < 0.01f) return;
        int ih6 = int(roundf(h*6-0.5f)+6)%6;
        ColorMapBPSW cm;
        cm.v = 1;
        cm.hue6 = ih6;
        float f = v*0.66f * (1 + (1-s)*1.0f);
        clamp01p(f);
        cm.getRGB(f, input.r(), input.g(), input.b());
    } else {
        h = clampf(v*0.5f*(1+mm->mrSlowBass->output), 0, 0.8333f);
        s = 1;
        ldColorUtil::hsv2rgb(h, s, v, input.r(), input.g(), input.b());
    }
}
