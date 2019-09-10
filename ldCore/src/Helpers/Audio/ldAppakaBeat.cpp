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
//  ldAppakaBeat.cpp
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//
#include "ldCore/Helpers/Audio/ldAppakaBeat.h"
#include <QtCore/QDebug>
#include <string>
#include "ldCore/Helpers/Maths/ldMathStat.h"

ldAppakaBeat::ldAppakaBeat()
{
    memset(peakRealTime, 0.0, buffersize * sizeof(float));
    memset(peakStage2, 0.0, buffersize * sizeof(float));
    memset(peakProba, 0.0, buffersize * sizeof(float));
    memset(tempBuffer, 0.0, buffersize * sizeof(float));
    memset(bufferData, 0.0, buffersize * sizeof(float));

    memset(powerData, 0.0, buffersize * sizeof(float));
    memset(powerDataNormed, 0.0, buffersize * sizeof(float));
    memset(virtualBeat, 0.0, buffersize * sizeof(float));
    //
    reset();
}

ldAppakaBeat::~ldAppakaBeat() { }

// reset
void ldAppakaBeat::reset()
{
    //qDebug() << " reset";
    bpm=120;
    isStarted=false;
    startCounter=0;
    peakRealTimeCount=0;
    peakProbaCount=0;
    peakStage2Count=0;
    elapsedPeakRealTimeDiff=0;
    stage2Count=0;
    output=0;
    outputRealTimePeak=0;
    resetBpmStat();
    resetDiff(peakRealTimeDiffs);
    resetDiff(peakProbaDiffs);
    resetDiff(peakStage2Diffs);
}

/**** RESET ****/

// resetBpmStat
void ldAppakaBeat::resetBpmStat()
{
    //qDebug() << " resetBpmStat";
    bestBpms.resize(maxBestBpms);
    for (int i=0;i<maxBestBpms; i++)
    {
        bestBpms[i].bpm=0.0;
        bestBpms[i].count=0;
        bestBpms[i].maxPathSum=0;
        bestBpms[i].rate=0;
        bestBpms[i].diff=0;
    }
}

// resetDiff
void ldAppakaBeat::resetDiff(std::vector<AppakaBeatDiffStruct> &diffs)
{
    // qDebug() << "resetDiff";
    diffs.resize(maxDiff);
    for (int i=0; i<maxDiff; i++)
    {
        diffs[i].count=0;
        diffs[i].maxIncrement=0;
        diffs[i].direct=false;
        diffs[i].maxMult=0;
        diffs[i].maxPath=0;
        diffs[i].bestBpm=0;
    }
}

/**** PROCESS ****/

// process
float ldAppakaBeat::process(ldSoundData* pSoundData)
{
    if (!pSoundData) {
        qDebug()<<"ldAppakaBeat no pSoundData";
        return 0;
    }

    // start
    virtualBeat[last_i]=0;
    peakStage2[last_i]=0;
    //peakHistory[last_i]=0;

    if (!isStarted) {
        startCounter++;
        if (startCounter > buffersize) isStarted=true;
    }

    // get sound power
    float val;
    val = 0;
    for (int i=0;i<300; i++) {
        float power=pSoundData->powerAt(i,300);
        if (60<i && i<100) val += power*power*power*power*power;
        else val += power*power*power;
        //if (val==0) qDebug()<<"ldAppakaBeat val 0";
    }
    //qDebug()<<"ldAppakaBeat val "<<val;
    // powerData
    //float average=ldMathStat::getFloatAverage(powerData, buffersize);
    //float deviation=ldMathStat::getFloatDeviation(powerData, buffersize, average);
    //float limiter = average + 5.0*deviation;
    //if (isStarted && val > limiter && limiter > 0) {
    // val=limiter;
    //}
    powerData[last_i]=val;

    if (isStarted) analyseRealTime();

    // output before buffer feeding
    outputRealTimePeak=peakRealTime[last_i];

    // feeds buffers
    for (int i = 1; i < buffersize; i++) {
        powerData[i-1]=powerData[i];
        peakRealTime[i-1]=peakRealTime[i];
        peakStage2[i-1]=peakStage2[i];
        virtualBeat[i-1]=virtualBeat[i];
    }
    if (isStarted) {
        //qDebug()<<"process start";
        analyse();
        // clean bpms
        makeStatBpm();
        cleanBpms();
        buildFirstVirtual();
    } //else qDebug()<<"not started"<< "startCounter "<<startCounter<< "buffersize "<<buffersize;
    //qDebug()<<"bpm"<<bpm;
    //qDebug()<<"startCounter"<<startCounter <<"floorf" <<floorf(1.0*startCounter/10.0);

    if (!isStarted && (startCounter%(15*AUDIO_OVERDRIVE_FACTOR)) == 0) {
       //qDebug() <<" ok => "<<startCounter;
       // return 1.0;
       int rd = rand() % 50;
       output=(rd==0)?1:0;
    } else output=virtualBeat[last_i];

    return output;
}

// analyseRealTime
void ldAppakaBeat::analyseRealTime()
{
    //qDebug()<<"analyseRealTime";
    resetDiff(peakRealTimeDiffs);
    // peakRealTime
    buildPeakRealTime();
    //
    peakRealTimeCount=ldMathStat::countNotNullValues(peakRealTime, buffersize);
    if (peakRealTimeCount>0) {
        // peakVect
        peakRealTimeVect.resize(peakRealTimeCount);
        buildPeakVect(peakRealTimeVect, peakRealTimeCount, peakRealTime, buffersize);
        computeStats(peakRealTimeVect, peakRealTimeCount, peakRealTimeDiffs);
    } //else qDebug()<<"no peak";
}

// analyse
void ldAppakaBeat::analyse()
{
    //
    resetDiff(peakProbaDiffs);
    resetDiff(peakStage2Diffs);

    // peakProba
    buildPeakProba();

    peakProbaCount=ldMathStat::countNotNullValues(peakProba, buffersize);
    if (peakProbaCount) {
        peakProbaVect.resize(peakProbaCount);
        buildPeakVect(peakProbaVect, peakProbaCount, peakProba, buffersize);
        computeStats(peakProbaVect, peakProbaCount, peakProbaDiffs);
    }
    //qDebug()<<"analyse Stage2";
    peakStage2Count=ldMathStat::countNotNullValues(peakStage2, buffersize);
    if (peakStage2Count) {
        peakStage2Vect.resize(peakStage2Count);
        buildPeakVect(peakStage2Vect, peakStage2Count, peakStage2, buffersize);
        computeStats(peakStage2Vect, peakStage2Count, peakStage2Diffs);
    }
}

/**** STATS *****/

// computeStats
void ldAppakaBeat::computeStats(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs)
{
    // peakRealTimeCount
    buildDiffs(peaks, size, diffs);
    computeMaxIncrements(peaks, size, diffs);
    computeBestBpm(peaks, size, diffs);
    //if (peakRealTimeCount>1) findConsecutivBpm(peakVect,peakRealTimeCount);
    printBestDiff(diffs);
    // findTest(peakVect,countPeak);
}

// buildDiffs
void ldAppakaBeat::buildDiffs(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs)
{
    int a;
    for (int i = 0; i < size; i++) {
        a = peaks[i].diffFromPrevious;
        if (a==0) continue;
		//a = MIN(MAX(0, a, maxDiff - 1));
		if (!(a>minDiff && a < maxDiff)) continue;
        diffs[a].count++;
        diffs[a].direct=true;
        //
        for (int j=0; j<size; j++)
        {
            int d=peaks[j].peakI-peaks[i].peakI;
            if (d>minDiff && d<maxDiff) {
                diffs[d].count++;
            }
        }
    }
}

// computeMaxIncrements
void ldAppakaBeat::computeMaxIncrements(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs)
{
    uint a,b,c,d,e;
    for (uint i = 3; i < (uint)size; i++) {
        a = 0;
        b = 0;
        c = 0;
//        d = 0;
//        e = 0;
        if (i<peaks.size()) a = peaks[i].diffFromPrevious;
        if ((i-1)<peaks.size()) b = peaks[i-1].diffFromPrevious;
        if ((i-2)<peaks.size()) c = peaks[i-2].diffFromPrevious;
        d=a+b;
        e=c+b;
        if (a==0 || b==0 || c==0) continue;
        int multAB,multBC,multAC,multAE,multDC;
        multAB=getMultiplier(a,b);
        multBC=getMultiplier(b,c);
        multAC=getMultiplier(a,c);
        multAE=getMultiplier(a,e);
        multDC=getMultiplier(d,c);
        if (diffs.size() > a) diffs[a].maxIncrement+=multAB+multAC+multAE;
        if (diffs.size() > b) diffs[b].maxIncrement+=multAB+multBC;
        if (diffs.size() > c) diffs[c].maxIncrement+=multBC+multBC+multDC;
        if (diffs.size() > d) diffs[d].maxIncrement+=multDC;
        if (diffs.size() > e) diffs[e].maxIncrement+=multAE;
    }
}

// computeBestBpm
void ldAppakaBeat::computeBestBpm(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs)
{
    int a,b;
    int maxPath=0;
    int maxMult=0;
    for (int i = 2; i < size; i++) {
        bool ok=false;
        a = peaks[i].diffFromPrevious;
        b = peaks[i-1].diffFromPrevious;
        if (a==0 || b==0) continue;
        if (a>=maxDiff || b>=maxDiff) continue;
        if ( a<minDiff ||  b<minDiff) continue;
        int mult=getMultiplier(a,b);
        //qDebug()<<"->"<<b<<" - "<<a<<" mult"<<mult<<" maxMult"<<maxMult<<" maxPath"<<maxPath;
        if (mult>0) {
            ok=true;
            maxPath+=b;
            maxPath+=a;
            maxMult+=(mult+1);
            //qDebug()<<"maxPath"<<maxPath<<"maxMult"<<maxMult;
            if (maxMult>diffs[a].maxMult && maxPath>diffs[a].maxPath) {
                diffs[a].maxMult=maxMult;
                diffs[a].maxPath=maxPath;
                //qDebug()<<"(1.0*maxPath/mult)"<<(1.0*maxPath/maxMult)<<"bestBpm"<<60.0*fps/(1.0*maxPath/maxMult);
                if (maxMult>0 && maxPath>0) diffs[a].bestBpm=60.0*fps/(1.0*maxPath/maxMult);
            }
        }
        if (!ok || i==(size-1)) {
             maxPath=0;
             maxMult=0;
        }
    }
}

// printBestDiff
void ldAppakaBeat::printBestDiff(std::vector<AppakaBeatDiffStruct> &diffs)
{
    //qDebug() << "printBestDiff";
    float maxProd=0;
    float prod=0;
    int maxDiffFound=-1;
    for (int a = minDiff; a < maxDiff; a++) {
        //qDebug() <<" a"<< a<<" count"<< diffs[a].count;
        prod=diffs[a].count*diffs[a].maxIncrement;
        if (diffs[a].maxPath<10) continue;
        if (prod>=maxProd) {
            maxProd=prod;
            maxDiffFound=a;
        }
        //qDebug() <<" a"<< a<<" count"<< diffs[a].count<< " maxMult"<< diffs[a].maxMult<<" maxPath"<< diffs[a].maxPath<<" bestBpm"<< diffs[a].bestBpm<<"diffs[a].maxIncrement"<<diffs[a].maxIncrement;
    }
    if (maxDiffFound == -1) maxDiffFound = minDiff;//(maxDiff -  minDiff)/2;
    //qDebug() <<" printBestDiff"<< maxDiffFound<<" bpm"<<;
    float bpm_ = diffs[maxDiffFound].bestBpm;
    if (bpm_>40.0 && bpm_ < 220) addNewBpm(bpm_, diffs[maxDiffFound].maxIncrement);
}


/***** BPM *****/

// indiceForBpm
int ldAppakaBeat::indiceForBpm(float b)
{
    for (int i=0;i<maxBestBpms; i++)
    {
        if (bestBpms[i].bpm==b) return i;
    }
    return -1;
}

// indiceForLowestRateBpm
int ldAppakaBeat::indiceForLowestRateBpm()
{
    int lowest=0;
    int rate=bestBpms[lowest].rate;
    for (int i=1;i<maxBestBpms; i++)
    {
        if (bestBpms[i].rate<rate) {
            rate=bestBpms[i].rate;
            lowest=i;
        }
    }
    return lowest;
}

// indiceForHighestRateBpm
int ldAppakaBeat::indiceForHighestRateBpm()
{
    int highest=0;
    int rate=bestBpms[highest].rate;
    for (int i=1;i<maxBestBpms; i++)
    {
        if (bestBpms[i].rate>rate) {
            rate=bestBpms[i].rate;
            highest=i;
        }
    }
    return highest;
}

// addNewBpm
void ldAppakaBeat::addNewBpm(float b, int maxPathSum)
{
    int ind=indiceForBpm(b);
    //qDebug() << "addNewBpm:" << b << " maxPathSum" << maxPathSum;
    //float instantBpm=b;

    // new
    if (-1==ind) {
        int lowest=indiceForLowestRateBpm();
        //qDebug() << "lowest" << lowest;
        bestBpms[lowest].bpm=b;
        bestBpms[lowest].rate=maxPathSum;
        bestBpms[lowest].count=1;
        bestBpms[lowest].maxPathSum=maxPathSum;
        //qDebug() << "lowest end" << lowest;
    } else {
        //qDebug() << "bestBpms[ind] ind" << ind;
        bestBpms[ind].bpm=b;
        bestBpms[ind].rate+=maxPathSum;
        bestBpms[ind].count++;
        if (maxPathSum>bestBpms[ind].maxPathSum) bestBpms[ind].maxPathSum=maxPathSum;
    }
}

// cleanBpms
void ldAppakaBeat::cleanBpms()
{
    for (int i=0;i<maxBestBpms; i++)
    {
        for (int j=0;j<maxBestBpms; j++)
        {
            int bpmI,bpmJ;
            if (i==j) continue;
            bpmI = round(bestBpms[i].bpm);
            bpmJ = round(bestBpms[j].bpm);
            if (bpmI==0||bpmJ==0) continue;
            int iToKeep=-1;
            int iToErase=-1;
            int maxBpm=(bpmI>bpmJ)?bpmI:bpmJ;
            //int minBpm=(bpmI>bpmJ)?bpmI:bpmJ;
            int mult = getMultiplier(bpmI,bpmJ);
            float multTier = getMultiplierFloat(bpmI,bpmJ);
            if (mult>1 && (bestBpms[i].count>10||bestBpms[j].count>10)) {
                iToKeep=(bestBpms[i].rate>bestBpms[j].rate)?i:j;
                iToErase=(bestBpms[i].rate>bestBpms[j].rate)?j:i;
            } else if (multTier==1.5) {
                if (maxBpm>150) {
                    iToKeep=(bpmI<bpmJ)?i:j;
                    iToErase=(bpmI<bpmJ)?j:i;
                }
            }
            if (iToErase>-1) {
                bestBpms[iToKeep].rate+=bestBpms[iToErase].rate;
                bestBpms[iToKeep].maxPathSum+=bestBpms[iToErase].maxPathSum;
                bestBpms[iToErase].bpm=0;
                bestBpms[iToErase].maxPathSum=0;
                bestBpms[iToErase].rate=0;
                bestBpms[iToErase].rate=0;
            }
        }
    }
}

// makeStatBpm
void ldAppakaBeat::makeStatBpm()
{
    //
    float bpmAverage=0;
    int count=0;
    for (int i=0;i<maxBestBpms; i++)
    {
        if (bestBpms[i].bpm > 0) {
            bpmAverage+=bestBpms[i].bpm*bestBpms[i].count;
            count+=bestBpms[i].count;
        }
    }
    if (count>0) bpmAverage = bpmAverage/count;
    float var=0.0;
    for (int i=0; i<maxBestBpms; i++) {
         if (bestBpms[i].bpm > 0) var+=pow(bestBpms[i].bpm-bpmAverage, 2)*bestBpms[i].count;
    }
    if (count>0) var=var/count;
    var=sqrtf(var);
/*
    int countUsed=0;
    for (int i=0;i<maxBestBpms; i++)
    {
        if (bestBpms[i].bpm > 0) {
            countUsed++;
            qDebug() << "i" << i  << "bpm" << bestBpms[i].bpm << "rate" << bestBpms[i].rate << "count" << bestBpms[i].count << " " << bestBpms[i].maxPathSum;
        }
    }
  */
    for (int i=0;i<maxBestBpms; i++)
    {
        float bpmb=bestBpms[i].bpm;
        if (bpmb>160.0) bestBpms[i].bpm /= 2.0;
        else if (bpmb<50.0) bestBpms[i].bpm *= 2.0;
    }

    //qDebug() << "var"<<var;
    // set the bpm
    float new_bpm=bestBpms[indiceForHighestRateBpm()].bpm;
    if (var > 3.5) {
        resetBpmStat();
    } else if (var < 1.0 && new_bpm > 40 && new_bpm < 220) {
        if (bpm==new_bpm) bestBpms[indiceForHighestRateBpm()].count+=10;
        bpm=new_bpm;
        //qDebug() << "bpmAverage" << bpmAverage << "var" << var ;
    }

}

/**** build peaks *****/

// buildPeakVect
void ldAppakaBeat::buildPeakVect(std::vector<AppakaBeatPeakStruct> &beats, int countPeak, float buf[], int size)
{
    if (!countPeak || !size) return;
    //qDebug() << "buildPeakVect countPeak" << countPeak << "size" << size;
    int c=0;
    //bool lastWasTooSmall=false;
    for (int i = 0; i < size; i++) {
        //qDebug() << "i" << i << "buf[i]" << buf[i];
        if (buf[i]==0 || c>=countPeak) continue;
        beats[c].value=buf[i];
        beats[c].peakI=i;
        beats[c].diffFromPrevious=0;
        beats[c].isFirst=false;
        if (c==0) {
            beats[c].isFirst=true;
        } else {
            beats[c].diffFromPrevious=beats[c].peakI-beats[c-1].peakI;
            /*if (lastWasTooSmall) {
                beats[c].diffFromPrevious=beats[c].peakI-beats[c-2].peakI;
                lastWasTooSmall=false;
            }
            if (c<(countPeak-1) && c>1) {
                if (beats[c].diffFromPrevious<minDiff) {
                    beats[c].diffFromPrevious=beats[c].peakI-beats[c-2].peakI;
                    lastWasTooSmall=true;
                }
            }*/
            //qDebug() << "c" << c << "beats[c].diffFromPrevious" << beats[c].diffFromPrevious;
        }
        c++;
    }
}

// buildPeakRealTime
void ldAppakaBeat::buildPeakRealTime()
{
    //
    float maxValue;
    copyArray(powerData, bufferData, buffersize);
    maxValue=ldMathStat::getMaxFloatValue(bufferData, buffersize);
    normalizeBuffer(bufferData, buffersize, maxValue);
    //
    for (int i = 1; i < buffersize; i++) {
        tempBuffer[i]=bufferData[i]-bufferData[i-1];
    }
    //
    maxValue=ldMathStat::getMaxFloatValue(tempBuffer, buffersize);
    normalizeBuffer(tempBuffer, buffersize, maxValue);
    float averageEnergy=ldMathStat::getFloatAverage(tempBuffer, buffersize);
    float deviationEnergy=ldMathStat::getFloatDeviation(tempBuffer, buffersize, averageEnergy);
    if (elapsedPeakRealTimeDiff==0 && tempBuffer[last_i] > (averageEnergy + deviationEnergy*0.60)) {
        peakRealTime[last_i]=1;
        elapsedPeakRealTimeDiff++;
        if (tempBuffer[last_i] > (averageEnergy + deviationEnergy*1.50)) virtualBeat[last_i]+=1.0; // virtualBeat peak
    } else {
        peakRealTime[last_i]=0;
    }
    if (elapsedPeakRealTimeDiff>0) {
        elapsedPeakRealTimeDiff++;
        if (elapsedPeakRealTimeDiff>minDiff-1) elapsedPeakRealTimeDiff=0;
    }
}

// buildPeakProba
void ldAppakaBeat::buildPeakProba()
{
    float maxValue;

    // work in peakProba
    copyArray(powerData, powerDataNormed, buffersize);
    maxValue=ldMathStat::getMaxFloatValue(powerDataNormed, buffersize);
    normalizeBuffer(powerDataNormed, buffersize, maxValue);

    copyArray(powerDataNormed, peakProba, buffersize);

    //
    float ave,dev;
    ave=ldMathStat::getFloatAverage(peakProba, buffersize);
    dev=ldMathStat::getFloatDeviation(peakProba, buffersize, ave);

    // stage2
    copyArray(peakProba, bufferData, buffersize);
    buildPeakStage2();

    // peakProba create
    whiteBonus(peakProba, buffersize);
    limiter(peakProba, buffersize, ave*3.0);
    scaleBuffer(peakProba, buffersize);
    killFades(peakProba, buffersize);
    killUnder(peakProba, buffersize, dev);
    strongKillFades(peakProba, buffersize);
    //
    maxValue=ldMathStat::getMaxFloatValue(peakProba, buffersize);
    normalizeBuffer(peakProba, buffersize, maxValue);
    //ave=ldMathStat::getFloatAverage(peakProba, buffersize);
    //dev=ldMathStat::getFloatDeviation(peakProba, buffersize, ave);

    // addpeaks
    for (int i = 0; i < buffersize; i++) {
        if (peakProba[i]==0) peakProba[i]=peakRealTime[i];
    }
}

// buildPeakStage2
void ldAppakaBeat::buildPeakStage2()
{
    if (stage2Count>0) {
        stage2Count++;
        peakStage2[last_i]=0.0;
        if (stage2Count==maxStage2Decal) {
            stage2Count=0;
        }
        return;
    }
    //
    float tmp[maxStage2Decal];
    for (int i=0; i<maxStage2Decal; i++)
    {
        tmp[i]=bufferData[buffersize-maxStage2Decal+i];
    }
    float maxValue=ldMathStat::getMaxFloatValue(tmp, maxStage2Decal);
    // qDebug()  << "maxValue"<<maxValue;
    int indice=ldMathStat::getKeyForMaxFloatValue(tmp, maxStage2Decal);
    if (maxValue>0.3) {
        peakStage2[buffersize-maxStage2Decal+indice]=1.0;
        //virtualBeat[buffersize-maxStage2Decal+indice]+=1.0;
        virtualBeat[last_i+1-maxStage2Decal+indice]+=2.0;
    }
    stage2Count++;
}

// buildFirstVirtual
void ldAppakaBeat::buildFirstVirtual()
{
    int count = ldMathStat::countNotNullValues(virtualBeat, buffersize);
    if (count==0) return;
    if (bpm<10||bpm>220) return;
    int maxInd = ldMathStat::getKeyForMaxFloatValue(virtualBeat, buffersize);
    int d=round(60.0*fps/bpm);
    //qDebug() << "maxInd" << maxInd<< "buffersize" << buffersize;
    //qDebug() << "d" << d;
    for (int i=maxInd-1; i>=0; i--)
    {
        if ((maxInd-i)%d==0) {
            virtualBeat[i]=0.0;
        }
        else virtualBeat[i]=0;
    }
    for (int i=maxInd+1; i<buffersize; i++)
    {
        if ((i-maxInd)%d==0) {
            virtualBeat[i]=1.0;
        } else virtualBeat[i]=0;
    }

    // check some
    float ave=0;
    count = ldMathStat::countNotNullValues(virtualBeat, buffersize);
    if (count==0) return;
    for (int i=0; i<buffersize; i++)
    {
        ave+=virtualBeat[i]*(powerDataNormed[i]);
    }
    ave /= count;
    bestBpms[indiceForHighestRateBpm()].count*=floor(0.8+ave*0.5*0.4);
    //qDebug() << "(ave)"<<(ave);
    if (ave < 1.0/1000) resetBpmStat();
}

/**** ALGO ****/

// getMultiplierFloat
float ldAppakaBeat::getMultiplierFloat(int a, int b)
{
    int min,max;
    max=(a>b)?a:b;
    min=(a>b)?b:a;
    if (a==0||b==0) return 0;
    float approx=floor(100.0*max/min);
    if (approx>0) {
        if (fabs(1.0*approx - 150) > 2*tolerance) return 1.5;
    }
    return 0;
}

// getMultiplier
int ldAppakaBeat::getMultiplier(int a, int b)
{
    int min,max;
    max=(a>b)?a:b;
    min=(a>b)?b:a;
    if (a==0||b==0) return 0;
    int approx=round(1.0*max/min);
    if (approx>0) {
        if (abs(max-min*approx) > tolerance*approx) return 0;
        else return approx;
    }
    return 0;
}

/**** TREAT ****/

// killUnder
void ldAppakaBeat::killUnder(float *tab, int size, float limit)
{
    for (int i=0; i < size; i++) {
        if (tab[i] < limit) tab[i]=0.0;
    }
}

// limiter
void ldAppakaBeat::limiter(float *tab, int size, float limit)
{
    for (int i=0; i < size; i++) {
        if (tab[i] > limit) tab[i]=limit;
    }
}

// killFades
void ldAppakaBeat::killFades(float *tab, int size)
{
    // straight
    for (int i=size-1; i > 0; i--) {
        if (tab[i] < tab[i-1]) tab[i]*=0.5;
    }
    for (int i=size-1; i > 0; i--) {
        if (tab[i] > tab[i-1]) tab[i-1]*=0.5;
    }
}

// strongKillFades
void ldAppakaBeat::strongKillFades(float *tab, int size)
{
    // limiter
    int limiter = 0;
    float _average=ldMathStat::getFloatAverage(tab, size);
    float lastMax=_average;
    for (int i=0; i < size; i++) {
        if (tab[i] > lastMax) {
            lastMax=tab[i];
            limiter=minDiff;
        } else if (limiter>0) {
            if (tab[i] <= lastMax) tab[i]=0.0;
            if (limiter==1) lastMax=_average;
            limiter--;
        }
    }
    lastMax=_average;
    limiter = 0;
    for (int i=size-1; i >= 0; i--) {
        if (tab[i] > lastMax) {
            lastMax=tab[i];
            limiter=minDiff;
        } else if (limiter>0) {
            if (tab[i] <= lastMax) tab[i]=0.0;
            if (limiter==1) lastMax=_average;
            limiter--;
        }
    }
}

// whiteBonus
void ldAppakaBeat::whiteBonus(float *tab, int size)
{
    float _average=ldMathStat::getFloatAverage(tab, size);
    float whiteCoef=1.0;
    for (int i = 0; i < size; i++) {
        if (tab[i] < _average) tab[i]=0.0;
        if (tab[i]==0) whiteCoef+=0.1f;
        else {
            tab[i] = tab[i] * whiteCoef;
            whiteCoef=1.0;
        }
    }
}

/**** STAT ****/

// normalizeBuffer
void ldAppakaBeat::normalizeBuffer(float *tab, int size, float ratio)
{
    if (ratio==0) ratio=1.0;
    for (int i = 0; i < size; i++) tab[i]=tab[i]/ratio;
}

// scaleBuffer
void ldAppakaBeat::scaleBuffer(float *tab, int size)
{
    float min=ldMathStat::getMinFloatValue(tab,size);
    float max=ldMathStat::getMaxFloatValue(tab,size);
    if (min>=max) return;
    for (int i = 0; i < size; i++) tab[i] = (tab[i] - min)/(max-min);
}

// copyArray
void ldAppakaBeat::copyArray(float tab[], float *result, int size)
{
    for (int i = 0; i < size; i++) {
        result[i]=tab[i];
    }
}
