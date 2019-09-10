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
    for (int i = 0; i < BUFFER_SIZE; i++) {
        m_bpmAubio[i]=0;
        m_bpmAppakBeat[i]=0;
        m_bpmAppakPeak[i]=0;
    }
    //
}

// ~ldAppakBpmSelector
ldAppakBpmSelector::~ldAppakBpmSelector() { }

// process
void ldAppakBpmSelector::process(float aubioFastBpm, float appakaBeatBpm, float appakaPeakBpm)
{
    //
    m_bpmAubio[0]=aubioFastBpm;
    m_bpmAppakBeat[0]=appakaBeatBpm;
    m_bpmAppakPeak[0]=appakaPeakBpm;
    //
    bool isOneZero = false;
    for (int i = BUFFER_SIZE-1; i > 0; i--) {
        m_bpmAubio[i]=m_bpmAubio[i-1];
        m_bpmAppakBeat[i]=m_bpmAppakBeat[i-1];
        m_bpmAppakPeak[i]=m_bpmAppakPeak[i-1];
        if (m_bpmAubio[i] < 0.001 || m_bpmAppakBeat[i] < 0.001) isOneZero = true;
    }
    // not started
    if (isOneZero) {
        if (aubioFastBpm > 1) m_bestBpm = aubioFastBpm;
        else if (appakaBeatBpm > 1) m_bestBpm = appakaBeatBpm;
        else m_bestBpm = 38; // debug value
    }

    //
    doStats();

    // lastTrustableBPM
    if (m_lastTrustableBPM > 1) m_bestBpm = m_lastTrustableBPM;

    //qDebug() << "bestBpm:"<<bestBpm;
}

float ldAppakBpmSelector::bestBpm() const
{
    return m_bestBpm;
}

// doStats
void ldAppakBpmSelector::doStats()
{
    //
    float aveAubio = ldMathStat::getFloatNotNullAverage(m_bpmAubio, BUFFER_SIZE);
//    float devAubio = ldMathStat::getFloatDeviation(bpmAubio, buffersize, aveAubio);

    //
    float aveAppaBeat = ldMathStat::getFloatNotNullAverage(m_bpmAppakBeat, BUFFER_SIZE);
    float devAppaBeat = ldMathStat::getFloatDeviation(m_bpmAppakBeat, BUFFER_SIZE, aveAppaBeat);

    //
    float maxAppaPeak = ldMathStat::getMaxFloatValue(m_bpmAppakPeak, BUFFER_SIZE);
    float minAppaPeak = ldMathStat::getMinFloatValue(m_bpmAppakPeak, BUFFER_SIZE);
    float aveAppaPeak = ldMathStat::getFloatNotNullAverage(m_bpmAppakPeak, BUFFER_SIZE);
    float devAppaPeak = ldMathStat::getFloatDeviation(m_bpmAppakPeak, BUFFER_SIZE, aveAppaPeak);

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
            m_lastVeryTrustableBPM = (aveAubio + aveAppaBeat)/2;
            m_lastTrustableBPM = m_lastVeryTrustableBPM;
            veryTrustableCase = true;
        } else if (devAppaBeat < smallDeviation) {
            m_lastSomehowTrustableBPM = (aveAubio + aveAppaBeat)/2;
            m_lastTrustableBPM = m_lastSomehowTrustableBPM;
            //qDebug() << "lastTrustableBPM 1" << lastTrustableBPM;
        } else { // something is wrong
            m_lastSomehowTrustableBPM = (aveAubio + aveAppaBeat)/2;
            m_lastTrustableBPM = m_lastSomehowTrustableBPM;
            //
            if (devAppaPeak < 2.0*smallDeviation && minAppaPeak > 2.0*aveAubio) {
                m_lastSomehowTrustableBPM = minAppaPeak/2;
                m_lastTrustableBPM = m_lastSomehowTrustableBPM;
            }
            //qDebug() << "lastTrustableBPM 1" << lastTrustableBPM;
        }
        isTrustable = true;
        if (debugIt) qDebug() << "A1] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // appak is not far double aveAppaPeak && aubio is 2/3 aveAppaBeat
    if (!isTrustable && ldMaths::isValueNearFrom(aveAppaPeak*2, aveAppaBeat, percentTrust) && devAppaPeak < smallDeviation
            && ldMaths::isValueNearFrom(2*aveAppaBeat/3.0, aveAubio, percentTrust) ) {
        m_lastSomehowTrustableBPM = aveAppaBeat;
        m_lastTrustableBPM = m_lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "A2] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // before autohrize special thing, we check that aubio is not close to lastVeryTrustableBPM
    if (!isTrustable && ldMaths::isValueNearFrom(m_lastVeryTrustableBPM, aveAubio, percentTrust) && minAppaPeak/2 < aveAubio) {
        m_lastVeryTrustableBPM = aveAubio;
        m_lastTrustableBPM = m_lastVeryTrustableBPM;
        isTrustable = true;
        aubioCheckedCase = true;
        if (debugIt) qDebug() << "B1] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // before autohrize special thing, we check that aubio is not close to minAppaPeak with big peak deviation
    if (!isTrustable && ldMaths::isValueNearFrom(minAppaPeak, aveAubio, 2*percentTrust) && devAppaPeak > 4.0*smallDeviation) {
        m_lastVeryTrustableBPM = aveAubio;
        m_lastTrustableBPM = m_lastVeryTrustableBPM;
        isTrustable = true;
        aubioCheckedCase = true;
        if (debugIt) qDebug() << "B2] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // appak is double aubio
    if ((!isTrustable||aubioCheckedCase) && ldMaths::isValueNearFrom(aveAppaBeat, aveAubio*2, percentTrust) && devAppaPeak < smallDeviation  && minAppaPeak > 1.5 * aveAppaBeat) {
        m_lastSomehowTrustableBPM = aveAppaBeat;
        m_lastTrustableBPM = m_lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C1] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // 3 appak is one Peak and 2 aubio is hal one peak -> half a peak
    if (!isTrustable && devAppaBeat < verySmallDeviation  && devAppaPeak < 2.5*smallDeviation
            &&  ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/3, 2.0*percentTrust)
            && (ldMaths::isValueNearFrom(aveAubio, aveAppaPeak/2, 2.0*percentTrust))
            ) {
        m_lastSomehowTrustableBPM = aveAubio*2;
        m_lastTrustableBPM = m_lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C2] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // appak is not far from lastVeryTrustableBPM
    if (!isTrustable && ldMaths::isValueNearFrom(m_lastVeryTrustableBPM, aveAppaBeat, percentTrust)) {
        m_lastSomehowTrustableBPM = aveAppaBeat;
        m_lastTrustableBPM = m_lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C3] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // lastSomehowTrustableBPM and aveAppaPeak/2 are close
    if (!isTrustable && ldMaths::isValueNearFrom(m_lastSomehowTrustableBPM, aveAppaPeak/2, percentTrust) && devAppaPeak < 2.0*smallDeviation) {
        m_lastTrustableBPM = m_lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C4] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // aubio and appak are far but appak deviation is low and not far aveAppaPeak or half aveAppaPeak
    if (!isTrustable && devAppaBeat < 2.0*verySmallDeviation
             && ( ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/3.0, percentTrust) || ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/2.0, percentTrust) || ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak, percentTrust) )
            ) {
        m_lastSomehowTrustableBPM = aveAppaBeat;
        m_lastTrustableBPM = m_lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C5] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // not trustable but 3 time aveAppaBeat is near aveAppaPeak and 2 time aveAubio is near aveAppaPeak
    if (!isTrustable && devAppaBeat < verySmallDeviation  && devAppaPeak < 2*smallDeviation
            &&  ldMaths::isValueNearFrom(aveAppaBeat, aveAppaPeak/3, 3.0*percentTrust)
            && (ldMaths::isValueNearFrom(aveAubio, aveAppaPeak/2, 2.0*percentTrust)
                || ldMaths::isValueNearFrom(aveAubio, aveAppaPeak/4, 2.0*percentTrust))
            ) {
        m_lastSomehowTrustableBPM = minAppaPeak/2;
        m_lastTrustableBPM = m_lastSomehowTrustableBPM;
        isTrustable = true;
        if (debugIt) qDebug() << "C6] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
    }

    // end: peaks are giving smthg very close to the double of the lastTrustableBPM
    if (isTrustable && m_lastTrustableBPM < 100 && !veryTrustableCase && ldMaths::isValueNearFrom(m_lastTrustableBPM, 0.5*aveAppaPeak, percentTrust) && devAppaPeak < smallDeviation) {
        m_lastTrustableBPM*=2.0;
        if (veryTrustableCase) m_lastVeryTrustableBPM*=2.0;
        else m_lastSomehowTrustableBPM*=2.0;
        if (debugIt) qDebug() << "x2 D1] lastTrustableBPM x 2" << m_lastTrustableBPM;
    }

    // some correction
    if (isTrustable && aubioCheckedCase && ldMaths::isValueNearFrom(m_lastSomehowTrustableBPM/2, m_lastVeryTrustableBPM, percentTrust)) {
        if (ldMaths::isValueNearFrom(aveAppaBeat/2, aveAubio, percentTrust)) {
            m_lastTrustableBPM*=2.0;
            m_lastVeryTrustableBPM*=2.0;
        }
        if (debugIt) qDebug() << "x2 D2] lastTrustableBPM x 2" << m_lastTrustableBPM;
    }

    // check
    if (isTrustable) {
        if (debugIt) qDebug() << "+++ END:OK] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
        if (debugIt) qDebug() << "           > aveAubio " << aveAubio << " aveAppaBeat " << aveAppaBeat << " devAppaBeat " << devAppaBeat << " minAppaPeak " << minAppaPeak << " maxAppaPeak " << maxAppaPeak << " aveAppaPeak " << aveAppaPeak  << " devAppaPeak " << devAppaPeak;
    } else {
        if (debugIt) qDebug() << "--- END:KO] lastTrustableBPM:" << m_lastTrustableBPM << " lastVeryTrustableBPM:" << m_lastVeryTrustableBPM << " lastSomehowTrustableBPM:" << m_lastSomehowTrustableBPM;
        if (debugIt) qDebug() << "               > aveAubio " << aveAubio << " aveAppaBeat " << aveAppaBeat << " devAppaBeat " << devAppaBeat << " minAppaPeak " << minAppaPeak << " maxAppaPeak " << maxAppaPeak << " aveAppaPeak " << aveAppaPeak  << " devAppaPeak " << devAppaPeak;
    }

}

