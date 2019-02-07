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

#include "ldCore/Visualizations/MusicManager/ldMusicManager.h"

#include <QtCore/QDebug>

#include <ldCore/Sound/ldSoundInterface.h>

#include "ldCore/Helpers/Audio/ldHybridReactor.h"
#include "ldCore/Helpers/Audio/ldTempoAC.h"
#include "ldCore/Helpers/Audio/ldAppakPeaks.h"
#include "ldCore/Helpers/Audio/ldAppakSpectrum.h"

// initial state
ldMusicManager::ldMusicManager(QObject* parent)
    : QObject(parent)
{
    qDebug() << __FUNCTION__;

    // soundgate
    if (isFakeSound) qDebug() << "WARNING!!! ldMusicManager isFakeSound is true! ";
    
    // spect
    // TODO: fix memory leak in spectFrame
    spectrogram.reset(new ldSpectrogram);
    spectrogram2.reset(new ldSpectrogram);

    spectAdvanced.reset(new ldSpectAdvanced(0.808f));
    spectAdvanced2.reset(new ldSpectAdvanced(0.303f));

    // tempo AC
    tempoACSlower.reset(new ldTempoAC(80/2.0/2.0, 5.00, 0.25, false));
    tempoACSlow.reset(new ldTempoAC(80/2.0/1.0, 4.00, 0.25, false));
    tempoACFast.reset(new ldTempoAC(90*1.3*1.0, 0.25, 0.75, false));
    tempoACFaster.reset(new ldTempoAC(90*1.3f*1.5f, 0.20f, 0.75f, false));


    // style detectors
    musicFeature1.reset(new MusicFeature1());

    tempoTrackerFast.reset(new ldTempoTracker((char*)"default", true, true, 0));
    tempoTrackerSlow.reset(new ldTempoTracker((char*)"default", false, true, 0));

    // music reactor
    mrSlowBass.reset(new ldMusicReactor());
    mrFastBass.reset(new ldMusicReactor());
    mrSlowTreb.reset(new ldMusicReactor());
    mrFastTreb.reset(new ldMusicReactor());
    mrVolume.reset(new ldMusicReactor());
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
    m_peaks.reset(new ldAppakPeaks());
    appakaGate.reset(new ldAppakGate());
    appakaSpectrum.reset(new ldAppakSpectrum());
    appakaBpmSelector.reset(new ldAppakBpmSelector());

    // audioBasic
    audioBasic.reset(new ldAudioBasic);
    
    // soundGate
    soundGate.reset(new ldSoundGate);
    silentThree.reset(new ldSilentThree);

    // drop detectors
    beatWarm.reset(new ldBeatWarm);
    beatFresh.reset(new ldBeatFresh);

    // pitch
//    pitchTracker = new PitchTracker();
    
    // hybrid
    hybridAnima.reset(new ldHybridAnima);
    hybridFlash.reset(new ldHybridFlash);
    hybridAutoColor2.reset(new ldHybridAutoColor2);
    hybridColorPalette.reset(new ldHybridColorPalette);
}


ldMusicManager::~ldMusicManager() {}

// process all algorithms
void ldMusicManager::updateWith(std::shared_ptr<ldSoundData> psd, float delta) {

    // first measurements
    m_psd = psd;
    audioBasic->process(psd.get());

    // basic music reactors
    mrVolume->process(psd.get());
    mrSlowBass->process(psd.get());
    mrFastBass->process(psd.get());
    mrSlowTreb->process(psd.get());
    mrFastTreb->process(psd.get());

    // Appak
    appakaBeat->process(psd.get());
    m_peaks->process(psd.get());
    appakaSpectrum->process(psd.get());

    // sound gate and silent
    soundGate->process(psd.get());
    silentThree->process(psd.get());
    appakaGate->basicMono(audioBasic->mono);

#ifndef LD_CORE_REDUCE_ANALYZER_SUPPORT
    // ******************
    // spectrum and spectrogram processing
    spectFrame.update(psd.get());
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
    bool ismusic = ((isSilent()?0:1) + (isSilent2()?0:1) + (silentThree?0:1)) > 0;
    tempoACSlower->update(spectrogram2.get(), ismusic);
    tempoACSlow->update(spectrogram2.get(), ismusic);
    tempoACFast->update(spectrogram2.get(), ismusic);
    tempoACFaster->update(spectrogram2.get(), ismusic);

    // aubio library tempo trackers
    // test fix
    tempoTrackerFast->process(psd.get());
    tempoTrackerSlow->process(psd.get());

    // feature metaprocessing
    musicFeature1->update(spectFrame, psd.get());

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
    //pitchTracker->update(psd.get(), onsetLargeBeat1, (onsetLargeBeat1 > 0.30), tempoACSlow->freqSmooth);
#endif

    // hybrid algos
    hybridAnima->process(this);
    hybridFlash->process(this);
    hybridAutoColor2->process(this);
    hybridColorPalette->process(this);

    // appak bpm selector
    appakaBpmSelector->process(tempoTrackerFast->bpm(), appakaBeat->bpm, m_peaks->lastBpmApproximation());

    m_peaks->processBpm(appakaBpmSelector->bestBpm, delta);
    emit updated();
}

float ldMusicManager::bass() const
{
    return m_psd ? m_psd->GetBass() : 0;
}

float ldMusicManager::mids() const
{
    return m_psd ? m_psd->GetMids() : 0;
}

float ldMusicManager::high() const
{
    return m_psd ? m_psd->GetHigh() : 0;
}

float ldMusicManager::volumePowerPre() const
{
    return m_psd ? m_psd->volumePowerPre : 0;
}

float ldMusicManager::volumePowerPost() const
{
    return m_psd ? m_psd->volumePowerPost : 0;
}

int ldMusicManager::soundLevel() const
{
    return m_psd ? m_psd->GetSoundLevel() : 0;
}

void ldMusicManager::setRealSoundLevel(int value)
{
    m_realSoundLevel = value;
}

int ldMusicManager::realSoundLevel() const
{
    return m_realSoundLevel;
}

const ldAppakPeaks *ldMusicManager::peaks() const
{
    return m_peaks.get();
}

bool ldMusicManager::isSilent() const
{
    if(isFakeSound)
        return false;

    return soundGate->isSilent;
}

bool ldMusicManager::isSilent2() const
{
    if(isFakeSound)
        return false;

    return mrVolume->isSilent2;
}

float ldMusicManager::isSilent2float() const
{
    if(isFakeSound)
        return 0;

    return mrVolume->isSilent2float;
}

bool ldMusicManager::isSilent3() const
{
    if(isFakeSound)
        return false;

    return silentThree->isSilent;
}

float ldMusicManager::bestBpm() const
{
    return appakaBpmSelector->bestBpm;
}

