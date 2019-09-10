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
//  ldVisualizerHelper.cpp
//  ldCore
//
//  Created by feldspar on 8/4/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Visualizer/ldBeatTracker.h"

#include <QtCore/QDebug>

/*!

  ldVisualizerHelper contains audio response classes for visualizers.

  Audio response classes contain low and high level abstractions of some common
  algorithms for animating based on musical data.  Many of the classes follow this
  pattern for usage:

  - Call a set function to provide parameters that define the behavior of the class.
  - Each time an audio frame is recieved, call an update function to give the class
    input data and process that data.
  - Read output variables of the class to get a value you can use to drive visualization.

  The most powerfull class right now is the MusicReactor class. This is the easiest way to
  get advanced music response in a visualizer.  PresetSignal is an earlier/simpler version
  of this class. BeatTracker, BeatSignal, and StatValue are sub-components that can be used
  individually for specific purposes.


*/

#include "ldCore/Helpers/Visualizer/ldVisualizerHelper.h"

#include <ldCore/Sound/ldSoundInterface.h>

#include <math.h>
#ifndef __MINGW32__
#ifndef __APPLE__
#ifndef ANDROID
#ifndef _MSC_VER
#ifndef Q_OS_LINUX
static float fminf(float a, float b) {return (a<b)?a:b;}
static float fmaxf(float a, float b) {return (a>b)?a:b;}
#endif
#endif
#endif
#endif
#endif


/*!
  \class BeatSignal
  \brief Provide linear scaling, shifting, and temporal decay.

  Setup:
  Modify mult, floor, and fade member variables to control process.
  mult: scale factor
  floor: ignore inputs below this value
  fade: decay exponent; modify this for a falling-off effect.

  Update:
  \l BeatSignal::process(float input, float delta)
  \param input : value of input signal
  \param delta : time in seconds since last frame (used for fade)

  Outputs:
  value: this member variable contains the processed signal.
  also, the return value of process function can be used.

*/

float ldBeatSignal::process(float input, float delta) {
    float current = (input - floor) * mult;
    current = fminf(current, 1);
    current = fmaxf(current, 0);
    value *= powf(fade, delta);
    //value = max(value, current);
    if (current > value) {
        value = current;
        if (!flipping) {
            flip = -flip;
            flipping = true;
        }
    } else {
        flipping = false;
    }
    return value;
}


/*!
  \class StatValue
  \brief Provide statistical normalization of an input

  This class

  Setup:
  \l StatValue::set(int _historysize), the parameter is the number of samples
  of input to buffer.  Larger values will respond more dramatically to dynamic
  changes in the music; smaller values are more consistent.

  Update:
  \l StatValue::add(float f), the paramater is an input signal.

  Outputs:
  scaledValue: The result of normalizing the input signal to its average. A value
  of 1 means the signal is at the average level.  A large beat that is 4x as loud
  will have a value of 4.
  probabilityValue: a value from 0-1 that correspondes to how many samples in the
  history buffer are below the input value.  If the current frame input value is
  larger than any previous input, this value is 1.  Provides a more steady response
  compared to scaledValue, with peaks that are smaller but more frequent.
  combinedValue: An alternative output equal to scaledValue * probabilityValue


*/

float ldStatValue::add(float f) {
    for (int i = historysize-1; i >= 1; i--) {
        history[i] = history[i-1];
    }
    history[0] = f;
    nfull++;
    if (nfull > historysize) nfull = historysize;
    float sum = 0;// prob
    float sum2 = 0;// average
    float total = 0;
    int ths = historysize;
    if (ths > nfull) ths = nfull;
    for (int i = 1; i < ths; i++) {
        float str = 1-(i+0.5)/ths;
        if (f > history[i]) sum += str*1;
        if (f == history[i]) sum += str*0.5;
        if (f < history[i]) sum += str*0;
        sum2 += str*history[i];
        total += str;
    }
    baseValue = f;
    if (total == 0) averageValue = 0; else averageValue = sum2 / total;
    if (averageValue == 0) scaledValue = 0; else scaledValue = baseValue / averageValue;
    if (total == 0) probabilityValue = 0.5; else probabilityValue = sum / total;
    combinedValue = scaledValue * probabilityValue;
    
    // dim start
    if (nfull < historysize/12) {
        float mult = ((float)nfull) / (float)(historysize / 12);
        scaledValue *= mult;
        probabilityValue *= mult;
        combinedValue *= mult;
    }
    
    return scaledValue;
}
void ldStatValue::set(int _historysize) {
    if (_historysize > maxstathistory) _historysize = maxstathistory;
    historysize = _historysize;
}


void ldPresetSignal::init() {

    float d = overdrive; // scale all time values by this
    stat.set(d*iscale(statTime, 20, 600)); // 2048 max

    signal.mult = fscale(signalMult, 0.f, 4.9f);
    signal.floor = fscale(signalFloor*signalFloor, 0.f, 0.9f);
    signal.fade = fscale(signalFade, 0.f, 0.9f);

    int trackerRangeStartValue = iscale(trackerRangeStart, 6, 30); // 240/2 max
    int trackerRangeEndValue = iscale(trackerRange, trackerRangeStartValue*2, 120); // 120 max
    tracker->set(d*trackerRangeStartValue,
                d*trackerRangeEndValue);



}


/*!
  \class PresetSignal
  \brief Deprecated - use MusicReactor

  Old version of MusicReactor class.

*/


float ldPresetSignal::processSignal(float f) {

//    float delta = 1.0/30.0;
//    delta /= overdrive;
    float delta = 1.0f/60.0f;
    //delta = 1.0/90.0;

    stat.add(f);
    statOutput = stat.scaledValue;
//    int statTypeValue = iscale(statType, 1, 3);
    int statTypeValue = 2;
    if (statType < 1.0/3.0) statTypeValue = 1;
    if (statType > 2.0/3.0) statTypeValue = 3;
    if (statTypeValue == 1)
        statOutput = stat.scaledValue/fscale(statParam, 1, 20);
    if (statTypeValue == 2)
        statOutput = stat.combinedValue/fscale(statParam, 1, 10);
    if (statTypeValue == 3)
        statOutput = powf(stat.probabilityValue, fscale(statParam, 1, 8));
    statOutput = clampf(statOutput, 0, 1);

    signal.process(statOutput, delta);
    signalOutput = signal.value;
    signalOutput = clampf(signalOutput, 0, 1);

    tracker->add(signalOutput);
    trackerOutput = tracker->bestphase();
    trackerOutput = clampf(trackerOutput, 0, 1);

    // calculate spin algorithms
    if (!spinning) {
        if (signal.value > trackerSpinThreshold && tracker->bestphase() < (1.0/16.0)) {
            spinning = true;
            spinspeed = tracker->bestbpm()*trackerSpinSpeedMult;
            spindir = signal.flip;
            if (spindir < 0) spinTurns = 1;
        }
    }
    if (spinning) {
        spinTurns += spinspeed * spindir;
        spinTurns4 += spinspeed * spindir / 4;
        if (spinTurns4 > 1) spinTurns4--;
        if (spinTurns4 < 0) spinTurns4++;
        if (spinTurns < 0 || spinTurns > 1) {
            spinning = 0;
            spinTurns = 0;
        }
    }
    spinOutput = spinTurns;

    // walk type spin algorithm
    //walkCounter -= (int) walkCounter;

    //walkcounter = tracker1.bestphase;
    walkCounter += tracker->bestbpm();
    float target = (tracker->bestphase()+4.0/32.0);
    target -= (int) target;
    float tdelta = target - walkCounter;
    if (tdelta < -0.5) tdelta++;
    if (tdelta > 0.5) tdelta--;
    float m = tracker->bestbpm() * 2.0 * walkWobbleFactor;
    if (tdelta > m) tdelta = m;
    if (tdelta < -m) tdelta = -m;
    walkCounter += tdelta;
    //walkcounter += tracker1.bestbpm/4;

    didClick = false;
    float t = 0;//1.0/32.0;
    if (/*owc > (1.0-t) &&*/ walkCounter > (1.0+t)) {
        didClick = true;
        walkCounter -= (int)walkCounter;
    }
    owc = walkCounter;

    walkerOutput = clampf(walkCounter, 0, 1);

    // return default output value
    output = signalOutput;
    return output;
}

ldPresetSignal::ldPresetSignal() :
    statOutput(0),
    signalOutput(0),
    tracker(new ldBeatTracker),
    trackerOutput(0),
    trackerSignalOutput(0),
    output(0),
    statTime(0.5),
    statType(0.5),
    statParam(0.5),
    signalMult(0.5),
    signalFloor (0.5),
    signalFade (0.5),
    trackerTime (0.5),
    trackerRangeStart (0.5),
    trackerRange (0.5),
    trackerSignalMult(0.5),
    trackerSignalFloor (0.5),
    trackerSignalFade(0.5)
  //stat(new stat())
{
    init();

    trackerSpinSpeedMult = 8.0f/7.0f;
    trackerSpinThreshold = 0.15f;
    spinning = false;
    spinspeed = 0;
    spindir = 1;
    spinTurns = 0;
    spinTurns4 = 0;
    spinOutput = 0;

    walkCounter = 0;
    walkWobbleFactor = 0.5;
    owc = 0;
    walkerOutput = 0;
    didClick = false;

}

ldPresetSignal::~ldPresetSignal()
{

}



/*!
  \class Music Reactor
  \brief A class that processes music data to a single output value.

  Setup:
  \l MusicReactor::setParams();
  argument is of type float[MRPARAMS]
  Each element in the array corresponds to a tunable variable with domain [0,1]
  This can be called at any time to change settings. It may take several seconds
  for the new values to take full effect.

  The values of these parameters are used to control internal settings of the
  processing.  This includes things like how quickly/slowly the value should
  respond to music, and what types of sounds to be sensitive to.

  //Example values for parameters
  float p1[MRPARAMS] = {0.50,0.09,0.79,0.46,0.31,0.45,0.49,0.20,0.25,0.32,0.16,0.67,0.45,0.45,1.76,0.25,0.2};
  float p2[MRPARAMS] = {0.50,0.39,0.89,0.36,0.31,0.75,0.49,0.20,0.25,0.22,0.21,0.47,0.45,0.45,0.22,0.65,0.3};
  float p3[MRPARAMS] = {0.50,0.79,0.99,0.26,0.21,0.75,0.49,0.20,0.25,0.12,0.26,0.27,0.25,0.25,0.44,0.75,0.4};

  Use these as a starting point and modify as needed.

  Processing:
  \l MusicReactor::process()
  The input parameter \param p is a ldSoundData object containing the new
  frame to be processed. Frames must contain exactly 1/(30hz * OVERDRIVE)
  of new data.

  Processing consists of applying the algorithms StatValue->BeatSignal->BeatTracker
  Input is taken from the ldSoundData object with a frequency range and other
  processing depending on parameters.

  Outputs:
  output: This variable contains the fully processed value.  This will be based
  on the output of one or more sub-algorithms, depending on params.

  Alternatively, you can access the output of the sub-algorithms directly:

  signalOutput: The result of the signal after normalization and processing, but
  no tempo detection. Use this for most pulsing or flashing type effects.
  trackerOutput: The raw phase result of tracker algorithm.  Very chaotic motion.
  spinOutput: Holds at zero until a beat, then smoothly raises to 1 over the duration.
  Good for spinning, or motion between 2 fixed endpoints.
  spinOutput4: Variant of spinOutput that holds at multiples of 1/4th.  Good for slower
  spinning (1/4 turn at a time).
  walkerOutput: Walker algorithm. Provides more reactiveness than spin algorithm but not
  as chaotic as raw tracker output.

*/


QString ldMusicReactor::labels[MRPARAMS] = {
"algo (sp4/sp/sig/walk/trac)",
"bass/treb", "bandwidth", "attack", "log",
"&stat history time", "&stat select", "&stat scale",
"&signal scale", "&signal lower bound", "&signal sustain", "&signal cutoff",
"&tracker history time", "&tracker fast/slow", "&tracker variability",
"&spin speedup factor", "&spin sensitivity", "&spin precision",
"&walker wobble factor", "&walker phase offset"
};

ldMusicReactor::ldMusicReactor()
    : tracker(new ldBeatTracker)
{
    for (int i = 0; i < MRPARAMS; i++) params[i] = 0.5f;
    updateParams();
    output = 0;
    lastInput = 0;
    statOutput = 0;
    signalOutput = 0;
    trackerOutput = 0;
    spinning = false;
    spinspeed = 0;
    spindir = 1;
    spinTurns = 0;
    spinTurns4 = 0;
    spinOutput = 0;
    spinOutput4 = 0;
    walkCounter = 0;
    owc = 0;
    walkerOutput = 0;
    walkerClickOutput = 0;
}

ldMusicReactor::~ldMusicReactor()
{

}

void ldMusicReactor::setParams(float _params[]) {
    for (int i = 0; i < MRPARAMS; i++) params[i] = clampf(_params[i], 0, 1);
    updateParams();
}


float ldMusicReactor::process(ldSoundData* p) {

    float d = overdrive; // scale all time values by this
    float delta = 1.0f/30.0f;
    delta /= overdrive;

    processStat(p, d);
    processSignal(delta);
    processTracker(d);
    processSpin();
    processWalk();

    // choose which algorithm to output as the primary "output" variable
    if (outputType < 1.0f/5.0f) output = spinOutput4;
    else if (outputType < 2.0f/5.0f) output = spinOutput;
    else if (outputType < 3.0f/5.0f) output = signalOutput;
    else if (outputType < 4.0f/5.0f) output = walkerOutput;
    else output = trackerOutput;

    isSilent2 = (output <= 1.0/8.0);
    isSilent2float = clampf((1 - output)*8-7, 0, 1);

    return output;
}


void ldMusicReactor::processStat(ldSoundData* p, float d)
{
    int nbins = iscale(frequencyRange, 2, 12);
    float fbin = expf(fscale(frequencyMid, 1, 5));
    fbin = (fbin - expf(1)) / (expf(5) - expf(1));
    int ibin = clampf(fbin, 0.f, 0.99f) * nbins;
    //float f = p->GetMids();
    float f = p->GetFFTValueForBlock(ibin, nbins);
    //printf("%i / %i\n", ibin, nbins);
    //static int c = 0; c++; c%=30; if (!c) qDebug() << ibin << " / " << nbins << "(" << frequencyMid << "+-" << frequencyRange;
    float currentInput = f;

    f = currentInput - (lastInput * slopeFactor);
    if (f < 0) f = 0;
    lastInput = currentInput;

    stat.set(d*iscale(statTime, 20, 600)); // 2048 max
 //statLog.set(d*iscale(statTime, 20, 600)); // 2048 max
    stat.add(f);

    // log factor
    //statLinear = stat;
    /*statLog.set(stat.historysize);
    statLog.add(log(f+1));
    float ta = (1-logFactor);
    float tb = (logFactor);
    stat.scaledValue = ta*stat.scaledValue + tb*statLog.scaledValue;
    stat.combinedValue = ta*stat.combinedValue + tb*statLog.combinedValue;
    stat.probabilityValue = ta*stat.probabilityValue + tb*statLog.probabilityValue;
*/

//        int statTypeValue = iscale(statType, 1, 3);
    int statTypeValue = 2;
    if (statType < 1.0/3.0) statTypeValue = 1;
    if (statType > 2.0/3.0) statTypeValue = 3;
    if (statTypeValue == 1)
        statOutput = stat.scaledValue/fscale(statParam, 1, 20);
    if (statTypeValue == 2)
        statOutput = stat.combinedValue/fscale(statParam, 1, 10);
    if (statTypeValue == 3)
        statOutput = powf(stat.probabilityValue, fscale(statParam, 1, 8));
    statOutput = clampf(statOutput, 0, 1);
    float ta = (1-logFactor);
    float tb = (logFactor);
    float logOutput = (log(statOutput+.1)-log(0+.1))
            /(log(1+.1)-log(0+.1));
    statOutput = ta*statOutput + tb*logOutput;
}

void ldMusicReactor::processSignal(float delta)
{
    signal.mult = fscale(signalMult, 0.f, 4.9f);
    signal.floor = fscale(signalFloor*signalFloor, 0.f, 0.9f);
    signal.fade = fscale(signalFade, 0.f, 0.9f);

    signal.process(statOutput, delta);
    signalOutput = signal.value;
    float floor = fscale(signalPostFloor, 0, 0.9f);
    signalOutput = (signalOutput-floor)/(1.0-floor);
    signalOutput = clampf(signalOutput, 0, 1);
}

void ldMusicReactor::processTracker(float d)
{
    int trackerRangeStartValue = iscale(trackerRangeStart, 6, 30); // 240/2 max
    int trackerRangeEndValue = iscale(trackerRange, trackerRangeStartValue*2, 120); // 120 max
    tracker->set(d*trackerRangeStartValue,
                d*trackerRangeEndValue);


    tracker->add(signalOutput);
    trackerOutput = tracker->bestphase();
    trackerOutput = clampf(trackerOutput, 0, 1);
}

void ldMusicReactor::processSpin()
{
    // calculate spin algorithms
    if (!spinning) {
        if (signal.value > spinThreshold && tracker->bestphase() < spinAngleThreshold) {
            spinning = true;
            spinspeed = tracker->bestbpm()*spinSpeedMult;
            if (spinspeed == 0) spinning = false;
            spindir = signal.flip;
            if (spindir < 0) spinTurns = 1;
        }
    }
    if (spinning) {
        spinTurns += spinspeed * spindir;
        spinTurns4 += spinspeed * spindir / 4;
        if (spinTurns4 > 1) spinTurns4--;
        if (spinTurns4 < 0) spinTurns4++;
        if (spinTurns < 0 || spinTurns > 1) {
            spinning = 0;
            spinTurns = 0;
        }
    }
    spinOutput = spinTurns;
    spinOutput4 = spinTurns4;
}

void ldMusicReactor::processWalk()
{
    // walk type spin algorithm
    walkCounter += tracker->bestbpm();
    float target = tracker->bestphase() + fscale(walkerAdvance, 0, 2*4.0/32.0);
    target -= (int) target;
    float tdelta = target - walkCounter;
    if (tdelta < -0.5) tdelta++;
    if (tdelta > 0.5) tdelta--;
    float m = tracker->bestbpm() * fscale(walkerWobbleFactor, 0.065f, 2.0f);
    if (tdelta > m) tdelta = m;
    if (tdelta < -m) tdelta = -m;
    walkCounter += tdelta;
    //walkcounter += tracker1.bestbpm/4;

    walkerClickOutput = 0;
    float t = 0;//1.0/32.0;
    if (/*owc > (1.0-t) &&*/ walkCounter > (1.0+t)) {
        walkerClickOutput = true;
        walkCounter -= (int)walkCounter;
    }
    owc = walkCounter;

    walkerOutput = clampf(walkCounter, 0, 1);
}

void ldMusicReactor::updateParams() {
    float* p[] = {&outputType,
                  &frequencyMid, &frequencyRange, &slopeFactor, &logFactor,
                  &statTime, &statType, &statParam,
                  &signalMult, &signalFloor, &signalFade, &signalPostFloor,
                  &trackerTime, &trackerRangeStart, &trackerRange,
                  &spinSpeedMult, &spinThreshold, &spinAngleThreshold,
                  &walkerWobbleFactor, &walkerAdvance};
    for (int i = 0; i < MRPARAMS; i++) *(p[i]) = params[i];

}
