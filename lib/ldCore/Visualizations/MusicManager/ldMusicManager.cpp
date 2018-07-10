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

#include "ldMusicManager.h"

#include <QtCore/QDebug>

#include <ldCore/Sound/ldSoundInterface.h>

#include "ldCore/Visualizations/util/AudioHelper/ldHybridReactor.h"
#include "ldCore/Visualizations/util/AudioHelper/ldTempoAC.h"

// initial state
ldMusicManager::ldMusicManager(QObject* parent)
    : QObject(parent)
{
    // soundgate
    if (isFakeSound) qDebug() << "WARNING!!! ldMusicManager isFakeSound is true! ";
    
    // spect
    // TODO: fix memory leak in spectFrame
    spectrogram.reset(new ldSpectrogram);
    spectrogram2.reset(new ldSpectrogram);

    spectAdvanced.reset(new SpectAdvanced(0.808f));
    spectAdvanced2.reset(new SpectAdvanced(0.303f));

    // tempo AC
    tempoACSlower.reset(new ldTempoAC(80/2.0/2.0, 5.00, 0.25, false));
    tempoACSlow.reset(new ldTempoAC(80/2.0/1.0, 4.00, 0.25, false));
    tempoACFast.reset(new ldTempoAC(90*1.3*1.0, 0.25, 0.75, false));
    tempoACFaster.reset(new ldTempoAC(90*1.3f*1.5f, 0.20f, 0.75f, false));


    // style detectors
    musicFeature1.reset(new MusicFeature1());

    // test fix
    if (doAubio) {
        tempoTrackerFast.reset(new TempoTracker((char*)"default", true, true, 0));
        tempoTrackerSlow.reset(new TempoTracker((char*)"default", false, true, 0));
    }

    // music reactor
    mrSlowBass.reset(new MusicReactor());
    mrFastBass.reset(new MusicReactor());
    mrSlowTreb.reset(new MusicReactor());
    mrFastTreb.reset(new MusicReactor());
    mrVolume.reset(new MusicReactor());
    //        p2[MRPARAMS] = {0.00f,0.00f,0.20f,0.00f,0.20f,0.83f,1.00f,0.63f,0.20f,0.49f,0.40f,0.09f,1.00f,0.75f,0.80f,0.62f,0.52f,0.09f,0.98f,0.00f};
    float   t_p2[MRPARAMS] = {0.50f,0.00f,0.20f,0.00f,0.20f,0.83f,1.00f,0.63f,0.20f,0.49f,0.40f,0.09f,0.98f,0.75f,1.00f,0.58f,0.50f,0.25f,0.50f,0.25f};

    //        fa[MRPARAMS] = {0.51f,0.00f,0.22f,0.00f,0.20f,1.00f,1.00f,0.26f,0.39f,0.21f,0.42f,0.66f,1.00f,0.77f,0.80f,0.48f,0.47f,0.46f,0.98f,0.00f};
    float   t_fa[MRPARAMS] = {0.75f,0.00f,0.34f,0.31f,0.14f,1.00f,1.00f,0.25f,0.56f,0.14f,0.40f,0.66f,0.98f,0.60f,0.75f,0.58f,0.37f,0.25f,1.00f,0.25f};

    //      t1fa[MRPARAMS] = {0.50f,0.79f,0.99f,0.26f,0.00f,0.21f,0.75f,0.49f,0.20f,0.25f,0.12f,0.26f,0.27f,0.85f,0.85f,0.25f,0.25f,0.25f,0.25f,0.25f};
    float t_t1fa[MRPARAMS] = {0.50f,0.79f,0.99f,0.26f,0.00f,0.21f,0.75f,0.49f,0.20f,0.25f,0.12f,0.26f,0.98f,0.40f,0.60f,0.58f,0.50f,0.25f,0.25f,0.25f};

    //       tp0[MRPARAMS] = {0.60f,0.00f,0.00f,0.75f,0.00f,0.50f,0.00f,0.27f,0.55f,0.15f,0.40f,0.15f,1.00f,1.00f,1.00f,0.46f,0.46f,0.46f,1.00f,1.00f};
    float  t_tp0[MRPARAMS] = {0.60f,0.00f,0.00f,0.75f,0.00f,0.50f,0.00f,0.27f,0.55f,0.15f,0.40f,0.15f,0.98f,0.20f,0.40f,0.58f,0.37f,0.25f,0.75f,0.25f};
    float pv[MRPARAMS] = {0.50f,0.36f,1.00f,0.25f,0.50f,1.00f,0.00f,0.93f,0.81f,0.05f,0.63f,0.18f,1.00f,0.75f,0.80f,0.46f,0.46f,0.46f,0.98f,0.00f};
    

    mrSlowBass->setParams(t_p2);
    mrFastBass->setParams(t_fa);
    mrSlowTreb->setParams(t_t1fa);
    mrFastTreb->setParams(t_tp0);
    mrVolume->setParams(pv);

    /*
    // tweak for real time
    mrSlowBass->params[19] = 0;
    mrFastBass->params[19] = 0;
    mrSlowTreb->params[19] = 0;
    mrFastTreb->params[19] = 0;
    mrVolume->params[19] = 0;*/  // no

    // appaka
    appakaBeat.reset(new ldAppakaBeat());
    appakaPeak.reset(new ldAppakPeaks());
    appakaGate.reset(new ldAppakGate());
    appakaSpectrum.reset(new ldAppakSpectrum());
    appakaBpmSelector.reset(new ldAppakBpmSelector());

    // audioBasic
    audioBasic.reset(new ldAudioBasic);
    
    // soundGate
    soundGate.reset(new ldSoundGate);
    silentThree.reset(new ldSilentThree);

    // drop detectors
    beatWarm.reset(new BeatWarm);
    beatFresh.reset(new BeatFresh);

    // pitch
//    pitchTracker = new PitchTracker();
    
    // hybrid
    hybridAnima.reset(new HybridAnima);
    hybridFlash.reset(new HybridFlash);
    hybridAutoColor2.reset(new HybridAutoColor2);
    hybridColorPalette.reset(new HybridColorPalette);
}


ldMusicManager::~ldMusicManager() {}

// process all algorithms

void ldMusicManager::updateWith(ldSoundData* psd, float /*delta*/) {

    // first measurements
    bass = psd->GetBass();
    mids = psd->GetMids();
    high = psd->GetHigh();
    volumePowerPre = psd->volumePowerPre;
    volumePowerPost = psd->volumePowerPost;
    audioBasic->process(psd);

    // basic music reactors
    mrVolume->process(psd);
    mrSlowBass->process(psd);
    mrFastBass->process(psd);
    mrSlowTreb->process(psd);
    mrFastTreb->process(psd);

    // Appak
    appakaBeat->process(psd);
    appakaPeak->process(psd);
    appakaSpectrum->process(psd);

    // sound gate and silent
    soundGate->process(psd);
    isSilent = soundGate->isSilent;
    isSilent2 = (mrVolume->output <= 1.0/8.0);
    isSilent2float = clampf((1 - mrVolume->output)*8-7, 0, 1);
    silentThree->process(psd);
    isSilent3 = silentThree->isSilent;
    appakaGate->basicMono(audioBasic->mono);

    // override silent if we want
    if (isFakeSound) {
        isSilent=false;
        isSilent2=false;
        isSilent2float=0;
        isSilent3=false;
    }
    // ******************
    // spectrum and spectrogram processing

#ifndef LD_USE_ANDROID_LAYOUT
    spectFrame.update(psd);
    spectrogram->addFrame(spectFrame);
    {
        ldSpectrumFrame t = spectrogram->getNoisedFrame(10, sqrt(2), sqrt(2), 1, 0);
        spectrogram2->addFrame(t);
    }
    spectFrame = spectrogram->currentFrame();
    //*spectFrame = spectrogram2->currentFrame;

    // spect advanced
    spectAdvanced->update(spectrogram2->currentFrame());
    spectAdvanced2->update(spectrogram2->currentFrame());

    // autocorrelative tempo processing
    spectrogram2->calculateS();
    bool ismusic = ((isSilent?0:1) + (isSilent2?0:1) + (silentThree?0:1)) > 0;
    tempoACSlower->update(spectrogram2.get(), ismusic);
    tempoACSlow->update(spectrogram2.get(), ismusic);
    tempoACFast->update(spectrogram2.get(), ismusic);
    tempoACFaster->update(spectrogram2.get(), ismusic);

    // aubio library tempo trackers
    // test fix
    if (doAubio) {
        tempoTrackerFast->process(psd);
        tempoTrackerSlow->process(psd);
    } else {
        tempoTrackerFast = NULL;
        tempoTrackerSlow = NULL;
    }

    // feature metaprocessing
    musicFeature1->update(spectFrame, psd);

    // *****************
    // drop detectors

    float ff4 = 0;
    ff4 += (sqrt(tempoACFast->phaseSmooth) + sqrt(tempoACSlow->phaseSmooth))/2;
    ff4 += MAX(mrFastBass->statOutput, mrFastTreb->statOutput);
    ff4 += MAX(mrSlowBass->statOutput, mrSlowTreb->statOutput);
    ff4 /= 3;
    ff4 = ff4*ff4*ff4;
    onsetLargeBeat1 = ff4;

    onsetLargeBeat2 = spectAdvanced2->peak2 - 1;
    onsetLargeBeat2 /= 2.717f;
    onsetLargeBeat2 = powf(onsetLargeBeat2, 1.0/2.0);
    clampfp(onsetLargeBeat2, 0, 1);

    beatWarm->update(spectAdvanced2.get(), spectAdvanced2.get(), tempoACFast->freqSmooth*1, tempoACSlow->freqSmooth*1);
    onsetBeatWarm = beatWarm->output;
    clampfp(onsetBeatWarm, 0, 1);

    beatFresh->update(spectrogram2->currentFrame().sls, (1/tempoACSlow->freqSmooth)/4);
    onsetBeatFresh = beatFresh->outputVar / 12;
    onsetBeatFresh = powf(onsetBeatFresh, 1/2.414f);
    clampfp(onsetBeatFresh, 0, 1);

    //if (onsetLargeBeat2 > 0.125) {
        //if (onsetBeatFresh > 0.0125 || onsetBeatWarm > 0.0125) {
            //wfr = onsetBeatFresh / (onsetBeatFresh + onsetBeatWarm);
            wfr = 0.5 + 2*(onsetLargeBeat2+1)*(onsetBeatFresh - onsetBeatWarm);
            dsewfr.rho = 0.212/(1+onsetLargeBeat2);
            //extern bool ABTEST;
            //if (ABTEST) dsewfr.rho = 0.0202/(1+onsetLargeBeat2);
            dsewfr.add(wfr);
            //wfr = dsewfr.mean;
        //}
    //}

    // pitch trackers
    //pitchTracker->update(psd, onsetLargeBeat1, (onsetLargeBeat1 > 0.30), tempoACSlow->freqSmooth);
#endif

    // hybrid algos
    hybridAnima->process(this);
    hybridFlash->process(this);
    hybridAutoColor2->process(this);
    hybridColorPalette->process(this);

    // appak bpm selector
    appakaBpmSelector->process(tempoTrackerFast->bpm, appakaBeat->bpm, appakaPeak->lastBmpApproximation);
    bestBpm2 = appakaBpmSelector->bestBpm;
    bestBpm = bestBpm2;

    emit updated();

    // lag test
    {
        float f = /*mrVolume->output +*/this->onsetLargeBeat2+this->onsetLargeBeat1;
        if (!lagTestInProgress) return;
        if (lagTestFramesDone >= lagTestDuration*lagTestFrames) {
            lagTestInProgress = false;
            emit lagTestAutoCompleted();
            qDebug() << "lag test result ms is " << lagTestResultMs
                     << ", confid " << lagTestResultConfidence;
            return;
        }

        lagTestFramesDone++;
        lagTestCand[(lagTestFramesDone - 1 + lagTestFrames)%lagTestFrames] += f;
        lagTestCand[(lagTestFramesDone     + lagTestFrames)%lagTestFrames] += f;
        lagTestCand[(lagTestFramesDone + 1 + lagTestFrames)%lagTestFrames] += f;

        int best = 0;
        float bestf = 0;
        float sumf = 0;
        float totalf = 0;
        for (int i = 0; i < lagTestFrames; i++) {
           float ff = 0;
           ff += lagTestCand[(i - 1 + lagTestFrames)%lagTestFrames];
           ff += lagTestCand[(i     + lagTestFrames)%lagTestFrames];
           ff += lagTestCand[(i + 1 + lagTestFrames)%lagTestFrames];
           if (ff > bestf) {
               bestf = ff;
               best = i;
           }
           sumf += ff;
           totalf += 1;
        }
        float averagef = sumf / totalf;
        float con = bestf / (bestf + averagef);
        //con *= 1.5f;
        con = sqrtf(con);
        int ms = best * 16.667f;// - 120;
        ms -= lagTestBaselineMs;
        if (ms < 0) ms += 1000;
        if (ms > 1000) ms -= 1000;
        if (ms > 750) {
            ms = 0;
            con = 0;
        }
        //qDebug() << (int)(f*1000) << "lag test result ms is " << ms << ", confid " << con;
        lagTestResultMs = ms;
        lagTestResultConfidence = con;
    }

}

void ldMusicManager::lagTestAutoStart() {
    lagTestInProgress = true;
    for (int i = 0; i < lagTestFrames; i++) {
       lagTestCand[i] = 0;
    }
    lagTestFramesDone = 0;
    lagTestBaselineMs = ldSoundInterface::s_latencyms;
}
