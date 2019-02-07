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

// ldAppakBpmSelector.cpp

#include "ldCore/Helpers/Audio/ldAppakBpmSelector.h"
#include <QtCore/QDebug>
#include "ldCore/Helpers/Maths/ldMathStat.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

// ldAppakBpmSelector
ldAppakBpmSelector::ldAppakBpmSelector()
{
    for (int i = 0; i < buffersize; i++) {
        bpmAubio[i]=0;
        bpmAppakBeat[i]=0;
        bpmAppakPeak[i]=0;
    }
    //
}

// ~ldAppakBpmSelector
ldAppakBpmSelector::~ldAppakBpmSelector() { }

// process
void ldAppakBpmSelector::process(float aubioFastBpm, float appakaBeatBpm, float appakaPeakBpm)
{
    //
    bpmAubio[0]=aubioFastBpm;
    bpmAppakBeat[0]=appakaBeatBpm;
    bpmAppakPeak[0]=appakaPeakBpm;
    //
    bool isOneZero = false;
    for (int i = buffersize-1; i > 0; i--) {
        bpmAubio[i]=bpmAubio[i-1];
        bpmAppakBeat[i]=bpmAppakBeat[i-1];
        bpmAppakPeak[i]=bpmAppakPeak[i-1];
        if (bpmAubio[i] < 0.001 || bpmAppakBeat[i] < 0.001) isOneZero = true;
    }
    // not started
    if (isOneZero) {
        if (aubioFastBpm > 1) bestBpm = aubioFastBpm;
        else if (appakaBeatBpm > 1) bestBpm = appakaBeatBpm;
        else bestBpm = 38; // debug value
    }

    //
    doStats();

    // lastTrustableBPM
    if (lastTrustableBPM > 1) bestBpm = lastTrustableBPM;

    //qDebug() << "bestBpm:"<<bestBpm;
}

// doStats
void ldAppakBpmSelector::doStats()
{
    //
    float aveAubio = ldMathStat::getFloatNotNullAverage(bpmAubio, buffersize);
//    float devAubio = ldMathStat::getFloatDeviation(bpmAubio, buffersize, aveAubio);

    //
    float aveAppaBeat = ldMathStat::getFloatNotNullAverage(bpmAppakBeat, buffersize);
    float devAppaBeat = ldMathStat::getFloatDeviation(bpmAppakBeat, buffersize, aveAppaBeat);

    //
    float maxAppaPeak = ldMathStat::getMaxFloatValue(bpmAppakPeak, buffersize);
    float minAppaPeak = ldMathStat::getMinFloatValue(bpmAppakPeak, buffersize);
    float aveAppaPeak = ldMathStat::getFloatNotNullAverage(bpmAppakPeak, buffersize);
    float devAppaPeak = ldMathStat::getFloatDeviation(bpmAppakPeak, buffersize, aveAppaPeak);

   // qDebug() << "far  aveAubio " << aveAubio << " aveAppaBeat " << aveAppaBeat << " devAppaBeat " << devAppaBeat << " minAppaPeak " << minAppaPeak << " maxAppaPeak " << maxAppaPeak << " aveAppaPeak " << aveAppaPeak  << " devAppaPeak " << devAppaPeak;

    float percentTrust = 5.0;
    float verySmallDeviation = 3.0;
    float smallDeviation = 15.0;
    bool isTrustable = false;
    bool veryTrustableCase = false;
    bool aubioCheckedCase = false;
    bool debugIt = false;

    // aubio and appak beat are close - ie easiest case
    if (ldMaths::isValueNearFrom(aveAubio, aveAppaBeat, percentTrust)) {
        if (devAppaBeat < verySmallDeviation) { // very trustable
            lastVeryTrustableBPM = (aveAubio + aveAppaBeat)/2;
            lastTrustableBPM = lastVeryTrustableBPM;
            veryTrustableCase = true;
        } else if (devAppaBeat < smallDeviation) {
            lastSomehowTrustableBPM = (aveAubio + aveAppaBeat)/2;
            lastTrustableBPM = lastSomehowTrustableBPM;
            //qDebug() << "lastTrustableBPM 1" << lastTrustableBPM;
        } else { // something is wrong
            lastSomehowTrustableBPM = (aveAubio + aveAppaBeat)/2;
            lastTrustableBPM = lastSomehowTrustableBPM;
            //
            if (devAppaPeak < 2.0*smallDeviation && minAppaPeak > 2.0*aveAubio) {
                lastSomehowTrustableBPM = minAppaPeak/2;
                lastTrustableBPM = lastSomehowTrustableBPM;
            }
            //qDebug() << "lastTrustableBPM 1" << lastTrustableBPM;
        }
        isTrustable = true;
        if (debugIt) qDebug() << "A1] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // appak is not far double aveAppaPeak && aubio is 2/3 aveAppaBeat
    if (!isTrustable && ldMaths::isValueNearFrom(aveAppaPeak*2, aveAppaBeat, percentTrust) && devAppaPeak < smallDeviation
            && ldMaths::isValueNearFrom(2*aveAppaBeat/3.0, aveAubio, percentTrust) ) {
        lastSomehowTrustableBPM = aveAppaBeat;
        lastTrustableBPM = lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "A2] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // before autohrize special thing, we check that aubio is not close to lastVeryTrustableBPM
    if (!isTrustable && ldMaths::isValueNearFrom(lastVeryTrustableBPM, aveAubio, percentTrust) && minAppaPeak/2 < aveAubio) {
        lastVeryTrustableBPM = aveAubio;
        lastTrustableBPM = lastVeryTrustableBPM;
        isTrustable = true;
        aubioCheckedCase = true;
        if (debugIt) qDebug() << "B1] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // before autohrize special thing, we check that aubio is not close to minAppaPeak with big peak deviation
    if (!isTrustable && ldMaths::isValueNearFrom(minAppaPeak, aveAubio, 2*percentTrust) && devAppaPeak > 4.0*smallDeviation) {
        lastVeryTrustableBPM = aveAubio;
        lastTrustableBPM = lastVeryTrustableBPM;
        isTrustable = true;
        aubioCheckedCase = true;
        if (debugIt) qDebug() << "B2] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // appak is double aubio
    if ((!isTrustable||aubioCheckedCase) && ldMaths::isValueNearFrom(aveAppaBeat, aveAubio*2, percentTrust) && devAppaPeak < smallDeviation  && minAppaPeak > 1.5 * aveAppaBeat) {
        lastSomehowTrustableBPM = aveAppaBeat;
        lastTrustableBPM = lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C1] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // 3 appak is one Peak and 2 aubio is hal one peak -> half a peak
    if (!isTrustable && devAppaBeat < verySmallDeviation  && devAppaPeak < 2.5*smallDeviation
            &&  ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/3, 2.0*percentTrust)
            && (ldMaths::isValueNearFrom(aveAubio, aveAppaPeak/2, 2.0*percentTrust))
            ) {
        lastSomehowTrustableBPM = aveAubio*2;
        lastTrustableBPM = lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C2] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // appak is not far from lastVeryTrustableBPM
    if (!isTrustable && ldMaths::isValueNearFrom(lastVeryTrustableBPM, aveAppaBeat, percentTrust)) {
        lastSomehowTrustableBPM = aveAppaBeat;
        lastTrustableBPM = lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C3] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // lastSomehowTrustableBPM and aveAppaPeak/2 are close
    if (!isTrustable && ldMaths::isValueNearFrom(lastSomehowTrustableBPM, aveAppaPeak/2, percentTrust) && devAppaPeak < 2.0*smallDeviation) {
        lastTrustableBPM = lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C4] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // aubio and appak are far but appak deviation is low and not far aveAppaPeak or half aveAppaPeak
    if (!isTrustable && devAppaBeat < 2.0*verySmallDeviation
             && ( ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/3.0, percentTrust) || ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/2.0, percentTrust) || ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak, percentTrust) )
            ) {
        lastSomehowTrustableBPM = aveAppaBeat;
        lastTrustableBPM = lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C5] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // not trustable but 3 time aveAppaBeat is near aveAppaPeak and 2 time aveAubio is near aveAppaPeak
    if (!isTrustable && devAppaBeat < verySmallDeviation  && devAppaPeak < 2*smallDeviation
            &&  ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/3, 3.0*percentTrust)
            && (ldMaths::isValueNearFrom(aveAubio, aveAppaPeak/2, 2.0*percentTrust)
                || ldMaths::isValueNearFrom(aveAubio, aveAppaPeak/4, 2.0*percentTrust))
            ) {
        lastSomehowTrustableBPM = minAppaPeak/2;
        lastTrustableBPM = lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C6] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
    }

    // end: peaks are giving smthg very close to the double of the lastTrustableBPM
    if (isTrustable && lastTrustableBPM < 100 && !veryTrustableCase && ldMaths::isValueNearFrom(lastTrustableBPM, 0.5*aveAppaPeak, percentTrust) && devAppaPeak < smallDeviation) {
        lastTrustableBPM*=2.0;
        if (veryTrustableCase) lastVeryTrustableBPM*=2.0;
        else lastSomehowTrustableBPM*=2.0;
        if (debugIt) qDebug() << "x2 D1] lastTrustableBPM x 2" << lastTrustableBPM;
    }

    // some correction
    if (isTrustable && aubioCheckedCase && ldMaths::isValueNearFrom(lastSomehowTrustableBPM/2, lastVeryTrustableBPM, percentTrust)) {
        if (ldMaths::isValueNearFrom(aveAppaBeat/2, aveAubio, percentTrust)) {
            lastTrustableBPM*=2.0;
            lastVeryTrustableBPM*=2.0;
        }
        if (debugIt) qDebug() << "x2 D2] lastTrustableBPM x 2" << lastTrustableBPM;
    }

    // check
    if (isTrustable) {
        if (debugIt) qDebug() << "+++ END:OK] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
        if (debugIt) qDebug() << "           > aveAubio " << aveAubio << " aveAppaBeat " << aveAppaBeat << " devAppaBeat " << devAppaBeat << " minAppaPeak " << minAppaPeak << " maxAppaPeak " << maxAppaPeak << " aveAppaPeak " << aveAppaPeak  << " devAppaPeak " << devAppaPeak;
    } else {
        if (debugIt) qDebug() << "--- END:KO] lastTrustableBPM:" << lastTrustableBPM << " lastVeryTrustableBPM:" << lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << lastSomehowTrustableBPM;
        if (debugIt) qDebug() << "               > aveAubio " << aveAubio << " aveAppaBeat " << aveAppaBeat << " devAppaBeat " << devAppaBeat << " minAppaPeak " << minAppaPeak << " maxAppaPeak " << maxAppaPeak << " aveAppaPeak " << aveAppaPeak  << " devAppaPeak " << devAppaPeak;
    }

}

