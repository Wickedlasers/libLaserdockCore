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
//  ldHybridReactor.cpp
//
//  Created by feld on 3/31/16.
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//
#include "ldCore/Helpers/Audio/ldHybridReactor.h"
#include <QtCore/QDebug>
#include <ldCore/Helpers/Color/ldColorUtil.h>
#include "ldCore/Helpers/Audio/ldTempoAC.h"


ldHybridFlash::ldHybridFlash() {
    for (int i = 0; i < 16; i++) {
        recent[i] = i;
        ison[i] = true;
    }
}

uint32_t ldHybridFlash::getColor(int i) {
    i %= 16;
    if (!ison[i]) return 0x000000;
    return colors[i];
}

uint32_t ldHybridFlash::getColor(int index, int max, int limit) {
    if (!ison[index]) return 0;
    int counted = 0;
    for (int i = 0; i < 16; i++) {
        int j = recent[i];
        if (j == index) {
            return getColor(index);
        }
        if (j < max) {
            if (ison[j]) {
                counted++;
                if (counted >= limit) return 0;
            }
        }
    }
    qDebug() << "error hybrid flash 4f920"; // should never reach this point
    return 0x00ff0000;
}


void ldHybridFlash::process(ldMusicManager* m) {
    
    float beatValue = (m->onsetBeatFresh + m->volumePowerPost() + m->tempoACFaster->phaseSmooth*0.5f)/2.5f;
    float bpf = m->tempoACSlow->freqSmooth;
    //static int erere = 0; erere++; if (erere % 10 == 0) qDebug() << bpf;

    static int last = 0;
    last++;
    if (last > 15) {
        if (beatValue > 0.15f && m->tempoTrackerSlow->output() == 1) {
            for (int i = 0; i < 8; i++) {
                int j = rand() % 16;
                int k = rand() % 16;
                int t = recent[j];
                recent[j] = recent[k];
                recent[k] = t;
            }
            last = 0;
        }
    }
    
    for (int i = 0; i < 16; i++) {

        float r1a = rand()/(RAND_MAX-1.0f) * 1.0f; if (rand()%2) r1a = 1;
        float r1b = rand()/(RAND_MAX-1.0f) * 1.0f; if (rand()%2) r1b = 0;
        float rba = rand()/(RAND_MAX-1.0f);
        float rch = rand()/(RAND_MAX-1.0f);
        selectorBeatAlgo[i].process(rba, 6, beatValue-r1a, 0.35f, 0.55f, 1.11f, bpf);
        selectorColorHue[i].process(rch, 3, beatValue*r1b, 0.25f, 0.65f, 1.11f, bpf/4.f);
        {
            int sbao = selectorBeatAlgo[i].indexOld;
            int sban = selectorBeatAlgo[i].indexNew;
            float sbaf = selectorBeatAlgo[i].indexProgress;
            float sbac = selectorBeatAlgo[i].indexProgressCompliment;

            int scho = selectorColorHue[i].indexOld;
            int schn = selectorColorHue[i].indexNew;
            float schf = selectorColorHue[i].indexProgress;
            float schc = selectorColorHue[i].indexProgressCompliment;

            //sbao = 1;
            //sban = 1;

            //scho = 1;
            //schn = 1;

            float v = 1;
            float vs[6];
            vs[0] = clampf(1-m->mrSlowTreb->walkerOutput*2, 0, 1);
            vs[1] = clampf(m->onsetBeatWarm+m->volumePowerPost()*2-1, 0, 1);
            vs[2] = clampf(m->mrFastTreb->output*2-1, 0, 1);
            vs[3] = rcost(m->tempoACSlower->phaseWalker);//1;//rcost(m->tempoACSlow->phaseSmooth);
            vs[4] = rcost(m->tempoACSlow->phaseSmooth);
            vs[5] = 1;//(m->tempoACFaster->phaseSmooth);//1;//clampf(m->onsetBeatWarm+m->volumePowerPost, 0, 1);
            float vo = vs[sbao];
            float vn = vs[sban];
            v = vn * sbaf + vo * sbac;
            //if (vn == 0) v = 0;

            float s = 1;
            float ss[6];
            ss[0] = 1;
            ss[1] = 1;//(m->tempoACFast->phaseSmooth)>0.5f?0:1; //1-vs[4];
            ss[2] = 1;
            ss[3] = 1;
            ss[4] = 1;//1-m->tempoACSlower->phaseSmooth;//rcost(m->tempoACSlower->phaseSmooth);//1;            
            ss[5] = 1-clampf(m->onsetBeatWarm+m->volumePowerPost()*2-1, 0, 1);
            float so = ss[sbao];
            float sn = ss[sban];
            s = sn * sbaf + so * sbac;

            float c = 0;
            float co = scho - 1;
            float cn = schn - 1;
            c = cn * schf + co * schc;            

            //s = v = 1;

            int h = 360 * (1.0f - 0.33f * c);
            colors[i] = ldColorUtil::colorHSV(h % 360, s, v);
            ison[i] = (v > 0.01f);
            //ison[i] = (sban != 0 && sban != 2);

            powers[i] = v;
            hues[i] = (h % 360) / 360.0f;


        }// else colors[i] = 0;
        //        ison[i] = (selectorBeatAlgo[i].indexNew != 0 || );
        //ison[i] = (colors[i] != 0);
    }   
        
}


ldHybridAnima::ldHybridAnima() {
}

void ldHybridAnima::process(ldMusicManager* m) {

    // selctor for hybrd algorithm
    float beatConfid = ((m->tempoACSlow->confidence + m->tempoACFast->confidence) / 2) * 1.0f;
    //beatConfid += (m->tempoTrackerSlow->confidence+m->tempoTrackerFast->confidence / 2) * 1.4f;
    beatConfid = beatConfid * 1.30 - 0.30f;
    clampfp(beatConfid, 0, 1);

    smooth2.rho = 0.15f;
    smooth2.add(beatConfid);
    beatConfid = smooth2.mean;
    clampfp(beatConfid, 0, 1);

    //qDebug() << "anim beat cofnfd " <<beatConfid;

    float beatValue = (m->onsetBeatFresh + m->volumePowerPost() + m->tempoACFaster->phaseSmooth/2)/3;
    float bpf = m->tempoACFast->bpmSmooth;
    selectorBeatAlgo.process(beatConfid, 4, beatValue, 0.40f, 0.60f, 1.11f, bpf);
    //qDebug() << beatValue;
    //qDebug() << beatConfid;

    float slowbeat = (m->onsetBeatWarm + m->mrVolume->output + m->volumePowerPost())/3;
    smooth1.rho = 0.15f;
    smooth1.add(slowbeat);
    slowbeat = clampf(smooth1.mean, 0, 1);
    slowbeat = 1.0 - 1.0 * slowbeat;

    float v1 = 0, v2 = 0;
    int s;

    s = selectorBeatAlgo.indexOld;
    //qDebug() << s;
    // FIXME 2 if are equal
    //    if (s >= 0) outputTrackPosition = slowbeat;//m->mrVolume->output;
    if (s >= 0) outputTrackPosition = 1-m->tempoACSlow->phaseSmooth;
    if (s >= 2) {outputTrackPosition = m->tempoACSlower->phaseWalker * 2; if (outputTrackPosition > 1) outputTrackPosition = 2-outputTrackPosition;}
    clampfp(outputTrackPosition, 0, 1);
    v1 = outputTrackPosition;

    s = selectorBeatAlgo.indexNew;
    //qDebug() << s;
    // FIXME 2 if are equal
    //    if (s >= 0) outputTrackPosition = slowbeat;//m->mrVolume->output;
    if (s >= 0) outputTrackPosition = 1-m->tempoACSlow->phaseSmooth;
    if (s >= 2) {outputTrackPosition = m->tempoACSlower->phaseWalker * 2; if (outputTrackPosition > 1) outputTrackPosition = 2-outputTrackPosition;}
    clampfp(outputTrackPosition, 0, 1);
    v2 = outputTrackPosition;

    //qDebug() << selectorBeatAlgo.indexProgress;
    outputTrackPosition = tweenf(v1, v2, selectorBeatAlgo.indexProgress);
    //qDebug() << "anim selector index " << selectorBeatAlgo.indexNew;

    // progress track position
    if (m->appakaGate->isSilent()) {
        //qDebug() << "anim silent";
        outputTrackPosition = 0;
    } /*else if (m->tempoACSlow->confidence < 0.25) {
      //qDebug() << "anim scratch effect";
      outputTrackPosition = 0.5 - 0.5*m->mrVolume->output; // if we use 1 we have problem of looping back to start frame
      } else if (m->tempoACSlower->confidence < 0.66) {
      //qDebug() << "anim walker";
      outputTrackPosition = m->mrFastTreb->walkerOutput;
      } else {
      //qDebug() << "anim reverse";
      outputTrackPosition = m->tempoACSlower->phaseSmooth * 2;
      if (outputTrackPosition > 1) outputTrackPosition = 2-outputTrackPosition;
      }*/






      // maybe change key frame
    bool newKey = false;

    // beats trigger new key frame
    if (m->onsetBeatFresh > 0.75) newKey = true;
    if (m->tempoACSlower->phaseSmooth >= 1) newKey = true;
    //if (m->tempoTrackerSlow->output >= 1) newKey = true;

    // dont change if lockout
    lockout--; if (lockout < 0) lockout = 0; else newKey = false;

    if (newKey) {
        // change key frame
        toggle = !toggle;
        if (m->musicFeature1->statMoodFast > 0.5) {
            // fast ones
            //qDebug() << "anim fast " << outputBaseFrameIndex;
            outputBaseFrameIndex = (toggle?1:0) + 2*(rand()%2); // 0-3 range
        } else {
            // slow ones
            //qDebug() << "anim slow " << outputBaseFrameIndex;
            outputBaseFrameIndex = toggle?5:4;
        }
        // lock out
        //        lockout = 2.0/(m->tempoACSlower->bpmSmooth); // 2 long beat
        lockout = 4.0 / ((m->tempoTrackerSlow->bpm() * 60.0f)*(AUDIO_UPDATE_DELTA_S));  // 4 aubio slow beats
        lockout = clampf(lockout, 2.0f/(AUDIO_UPDATE_DELTA_S), 10.0f/(AUDIO_UPDATE_DELTA_S)); // range 2-10s of lockout time
    }
    //qDebug() << "anim lockout " << lockout;

}

ldHybridAutoColor2::ldHybridAutoColor2() {
    outputColor1 = ldColorUtil::colorRGB(128, 255, 0);
    outputColor2 = ldColorUtil::colorRGB(0, 255, 255);
}

void ldHybridAutoColor2::process(ldMusicManager* m) {
    
    // selctor for hybrd algorithm
    float beatValue = m->onsetBeatFresh;
    float bpf = m->tempoACFaster->bpmInstant/2;
    
    float melody = m->musicFeature1->statMoodMelodic2 * 1.0;
    selectorColorHue1.process(melody, 4, beatValue, 0.45f, 0.85f, 0.97f, bpf/2.f);
    float funk = m->musicFeature1->statMoodFunky2 * 1.0;
    selectorColorHue2.process(funk, 5, beatValue, 0.45f, 0.80f, 0.96f, bpf);
    float fastness = m->musicFeature1->statMoodFast * 1.0;
    selectorColorSat2.process(fastness, 3, beatValue, 0.45f, 0.75f, 0.95f, bpf*2.f);
    
    
    outputColor1 = ldColorUtil::colorHSV(359*selectorColorHue1.indexFloat/4, 1, 1);
    outputColor2 = ldColorUtil::colorHSV(359*(selectorColorHue2.indexFloat+0.5)/5+0.1, selectorColorSat2.indexFloat/2, clampf(/*m->mrVolume->output*/+m->tempoACSlower->phaseReactive, 0.5, 1));
    
        
}


ldHybridColorPalette::ldHybridColorPalette() {
    
    //p, c, l
    
    p1[0][0][0] = 0.00;    p1[0][0][1] = 1.00;    p1[0][0][2] = 0.00;
    p1[0][1][0] = 0.50;    p1[0][1][1] = 1.00;    p1[0][1][2] = 0.50;
    p1[0][2][0] = 0.00;    p1[0][2][1] = 0.50;    p1[0][2][2] = 0.00;
    
    p1[1][0][0] = 1.00;    p1[1][0][1] = 0.00;    p1[1][0][2] = 0.00;
    p1[1][1][0] = 1.00;    p1[1][1][1] = 0.50;    p1[1][1][2] = 0.25;
    p1[1][2][0] = 0.75;    p1[1][2][1] = 0.75;    p1[1][2][2] = 0.00;
    
    p1[2][0][0] = 0.75;    p1[2][0][1] = 0.75;    p1[2][0][2] = 0.75;
    p1[2][1][0] = 0.00;    p1[2][1][1] = 0.00;    p1[2][1][2] = 1.00;
    p1[2][2][0] = 0.00;    p1[2][2][1] = 0.75;    p1[2][2][2] = 0.75;
    
    p1[3][0][0] = 0.25;    p1[3][0][1] = 1.00;    p1[3][0][2] = 0.25;
    p1[3][1][0] = 0.00;    p1[3][1][1] = 0.75;    p1[3][1][2] = 0.50;
    p1[3][2][0] = 0.00;    p1[3][2][1] = 0.00;    p1[3][2][2] = 1.00;
    
}

void ldHybridColorPalette::process(ldMusicManager* m) {
    /*float f = m->tempoACSlower->phaseSmooth;
    if (f == 1) {
        base = (base + 1) % 3;
        if (m->onsetBeatFresh > 0.75)
            base = (base + 1) % 3;
    }
    progress = 1.0 - f;*/
    
   /* float f = m->mrSlowBass->spinOutput4;
    base = (f*4);
    progress = clampf((f*4) - base, 0, 1);
    base = base % 4;
    
    static bool b = false;
    if (m->tempoACFaster->phaseSmooth == 1 && m->onsetBeatFresh > 0.6) b = !b;
    if (b) base = 3-base;*/
    
    progress = rcost(m->tempoACSlow->phaseSmooth);
    if (m->tempoACSlow  ->phaseSmooth == 1 && m->onsetLargeBeat1 > 0.25) base = (base + 1) % 4;
    if (m->tempoACFast  ->phaseSmooth == 1 && m->onsetBeatFresh > 0.5) base = (base + 1) % 4;
    
    
}

void ldHybridColorPalette::colorize(float& rr, float& gg, float& bb) {
    float r = rr;
    float g = gg;
    float b = bb;

    int i1 = base;
    int i2 = (base + 1) % 4;
    
    float p = progress;
    float s = 1.0f - progress;
    
    rr = r*(s*p1[i1][0][0] + p*p1[i2][0][0]) +
         g*(s*p1[i1][1][0] + p*p1[i2][1][0]) +
         b*(s*p1[i1][2][0] + p*p1[i2][2][0]);
    gg = r*(s*p1[i1][0][1] + p*p1[i2][0][1]) +
         g*(s*p1[i1][1][1] + p*p1[i2][1][1]) +
         b*(s*p1[i1][2][1] + p*p1[i2][2][1]);
    bb = r*(s*p1[i1][0][2] + p*p1[i2][0][2]) +
         g*(s*p1[i1][1][2] + p*p1[i2][1][2]) +
         b*(s*p1[i1][2][2] + p*p1[i2][2][2]);
    
    clampfp(rr, 0, 1);
    clampfp(gg, 0, 1);
    clampfp(bb, 0, 1);
}



