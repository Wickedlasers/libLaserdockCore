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
//  ldAppakaBeat.h
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//
#ifndef LDAPPAKABEAT_H
#define LDAPPAKABEAT_H

#include <ldCore/Sound/ldSoundData.h>

struct LDCORESHARED_EXPORT AppakaBeatPeakStruct {
    float value;
    int peakI;
    int diffFromPrevious;
    bool isFirst;
    float trust;
};

struct LDCORESHARED_EXPORT AppakaBeatDiffStruct {
    int count;
    int maxIncrement;
    bool direct;
    int maxPath;
    int maxMult;
    float bestBpm;
};

struct LDCORESHARED_EXPORT AppakaBeatBpmStruct {
    float bpm;
    int count;
    int maxPathSum;
    int rate;
    int diff;
};

//
typedef std::vector<AppakaBeatPeakStruct> appakabeat_vec_t;
typedef std::vector<AppakaBeatDiffStruct> appakabeatdiff_vec_t;
typedef std::vector<AppakaBeatBpmStruct> appakabeatbpm_vec_t;
typedef std::vector<int> appakabeat_int_vec_t;

class LDCORESHARED_EXPORT ldAppakaBeat
{
public:
    ldAppakaBeat();
    ~ldAppakaBeat();

    static const int fps = 30*AUDIO_OVERDRIVE_FACTOR;
    static const int seconds = 2;
    static const int buffersize = seconds*fps;
    static const int computeDecay = 5;
    static const int bufferstatssize = 1*seconds*fps;
    static const int minDiff = 9*AUDIO_OVERDRIVE_FACTOR; // means higher than 200 bpm at 30fps
    static const int maxDiff = 46*AUDIO_OVERDRIVE_FACTOR; // means lower than 40 bpm at 30fps
    static const int tolerance = 2*AUDIO_OVERDRIVE_FACTOR;
    static const int maxBestBpms = 20;
    static const int last_i = buffersize-1;
    static const int maxStage2Decal = 2*minDiff;

    int peakRealTimeCount,peakProbaCount,peakStage2Count;
    int stage2Count;

    float powerData[buffersize];
    float powerDataNormed[buffersize];
    float tempBuffer[buffersize];
    float bufferData[buffersize];
    float virtualBeat[buffersize];

    float peakRealTime[buffersize];
    float peakStage2[buffersize];
    float peakProba[buffersize];
    float elapsedPeakRealTimeDiff;

    float bpm;
    float output;
    float outputRealTimePeak;

    float process(ldSoundData* pSoundData);


private:
    int startCounter;
    bool isStarted;
    appakabeat_vec_t peakRealTimeVect,peakProbaVect,peakStage2Vect;
    appakabeatdiff_vec_t peakRealTimeDiffs,peakProbaDiffs,peakStage2Diffs;
    appakabeatbpm_vec_t bestBpms;

    void reset();
    void resetBpmStat();
    void resetDiff(std::vector<AppakaBeatDiffStruct> &diffs);

    void analyseRealTime();
    void analyse();

    void buildFirstVirtual();

    void buildPeakRealTime();
    void buildPeakProba();
    void buildPeakStage2();

    void computeStats(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs);
    void computeMaxIncrements(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs);
    void computeBestBpm(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs);

    void buildDiffs(std::vector<AppakaBeatPeakStruct> &peaks, int size, std::vector<AppakaBeatDiffStruct> &diffs);
    void printBestDiff(std::vector<AppakaBeatDiffStruct> &diffs);

    void makeStatPeaks(std::vector<AppakaBeatPeakStruct> &beats, int countPeak);
    void buildPeakVect(std::vector<AppakaBeatPeakStruct> &beats, int countPeak, float buf[], int size);
    void findConsecutivBpm(std::vector<AppakaBeatPeakStruct> &beats, int countPeak);
    void findTest(std::vector<AppakaBeatPeakStruct> &beats, int countPeak);
    float getPointForPeaks(std::vector<AppakaBeatPeakStruct> &beats, int countPeak, int diff);

    int indiceForBpm(float b);
    int indiceForLowestRateBpm();
    int indiceForHighestRateBpm();
    void addNewBpm(float b, int maxPathSum);

    void cleanBpms();
    void makeStatBpm();

    void killUnder(float *tab, int size, float limit);
    void limiter(float *tab, int size, float limit);
    void killFades(float *tab, int size);
    void strongKillFades(float *tab, int size);
    void whiteBonus(float *tab, int size);

    //
    float getMultiplierFloat(int a, int b);
    int getMultiplier(int a, int b);
    //
    void copyArray(float tab[], float *result, int size);
    void normalizeBuffer(float *tab, int size, float ratio);
    void scaleBuffer(float *tab, int size);
};

#endif // LDAPPAKABEAT_H
