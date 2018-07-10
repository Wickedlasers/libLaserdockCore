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

#ifndef LDMUSICMANAGER_H
#define LDMUSICMANAGER_H

#include <memory>

#include "ldCore/Visualizations/util/AudioHelper/ldAppakaBeat.h"
#include "ldCore/Visualizations/util/AudioHelper/ldAppakPeaks.h"
#include "ldCore/Visualizations/util/AudioHelper/ldAppakSpectrum.h"
#include "ldCore/Visualizations/util/AudioHelper/ldAppakGate.h"
#include "ldCore/Visualizations/util/AudioHelper/ldAppakBpmSelector.h"
#include "ldCore/Visualizations/util/AudioHelper/ldAudioBasic.h"
#include "ldCore/Visualizations/util/AudioHelper/ldDurationalStatEstimator.h"
#include "ldCore/Visualizations/util/AudioHelper/ldMusicFeature.h"
#include "ldCore/Visualizations/util/AudioHelper/ldSoundGate.h"
#include "ldCore/Visualizations/util/AudioHelper/ldSilentThree.h"
#include "ldCore/Visualizations/util/AudioHelper/ldSpectrumFrame.h"
#include "ldCore/Visualizations/util/AudioHelper/ldTempoTracker.h"
#include "ldCore/Visualizations/util/VisualizerHelper/ldVisualizerHelper.h"

class ldSpectrogram;
class HybridAnima;
class HybridFlash;
class HybridAutoColor2;
class HybridColorPalette;
class ldTempoAC;

class LDCORESHARED_EXPORT ldMusicManager : public QObject
{
    Q_OBJECT

public:
    // class stuff
    ldMusicManager(QObject* parent);
    ~ldMusicManager();

    // update function
    void updateWith(ldSoundData* psd, float delta);

    // doAubio
#ifndef __APPLE__
    #define doAubio 1
#else
    #define doAubio 1
#endif
    // beat variables
    // basic funcs
    float bass = 0.f, mids = 0.f, high = 0.f;
    float volumePowerPre = 0.f, volumePowerPost = 0.f;
    ldSpectrumFrame spectFrame;
    std::unique_ptr<ldSpectrogram> spectrogram;
    std::unique_ptr<ldSpectrogram> spectrogram2;
    std::unique_ptr<SpectAdvanced> spectAdvanced;
    std::unique_ptr<SpectAdvanced> spectAdvanced2;
    std::unique_ptr<ldTempoAC> tempoACSlower;
    std::unique_ptr<ldTempoAC> tempoACSlow;
    std::unique_ptr<ldTempoAC> tempoACFast;
    std::unique_ptr<ldTempoAC> tempoACFaster;
    std::unique_ptr<MusicFeature1> musicFeature1;

    std::unique_ptr<MusicReactor> mrSlowBass;
    std::unique_ptr<MusicReactor> mrFastBass;
    std::unique_ptr<MusicReactor> mrSlowTreb;
    std::unique_ptr<MusicReactor> mrFastTreb;
    std::unique_ptr<MusicReactor> mrVolume;

    std::shared_ptr<TempoTracker> tempoTrackerFast;
    std::unique_ptr<TempoTracker> tempoTrackerSlow;

    //
    std::unique_ptr<ldAppakaBeat> appakaBeat;
    std::unique_ptr<ldAppakPeaks> appakaPeak;
    std::unique_ptr<ldAppakGate> appakaGate;
    std::unique_ptr<ldAppakSpectrum> appakaSpectrum;
    std::unique_ptr<ldAppakBpmSelector> appakaBpmSelector;

    //
    std::unique_ptr<ldAudioBasic> audioBasic ;
    
    // sound gate
    std::unique_ptr<ldSoundGate> soundGate;
    bool isSilent = true;
    bool isSilent2 = true;
    float isSilent2float = 1.f;
    std::unique_ptr<ldSilentThree> silentThree;
    bool isSilent3 = true;
    bool isFakeSound = false; // kill isSilent (shall be false in production!)

	// onset
    float onsetLargeBeat1 = 0.f;
    float onsetLargeBeat2 = 0.f;
    float onsetBeatWarm = 0.f;
    float onsetBeatFresh = 0.f;
    std::unique_ptr<BeatWarm> beatWarm;
    std::unique_ptr<BeatFresh> beatFresh;
    float wfr = 0.f;
    DurationalStatEstimator dsewfr;

	// pitch
//    PitchTracker* pitchTracker = nullptr; // deprecated?
    
    // hybrids;
    std::unique_ptr<HybridAnima> hybridAnima;
    std::unique_ptr<HybridAutoColor2> hybridAutoColor2;
    std::unique_ptr<HybridColorPalette> hybridColorPalette;
    std::unique_ptr<HybridFlash> hybridFlash;

    float bestBpm = 120.f;
    float bestBpm2 = 0.f;

    //

signals:
    void updated();


    // lag test

signals:
    void lagTestAutoCompleted();
public slots:
    void lagTestAutoStart();
public:
    int lagTestResultMs = 250;
    float lagTestResultConfidence = 0;
    //
    bool lagTestInProgress = false;
    static const int lagTestFrames = 30 * AUDIO_OVERDRIVE_FACTOR; // range width in audio frames
    static const int lagTestDuration = 4;
    float lagTestCand[lagTestFrames];
    int lagTestFramesDone = 0;
    int lagTestBaselineMs = 0;

private:
};

#endif // LDMUSICMANAGER_H
