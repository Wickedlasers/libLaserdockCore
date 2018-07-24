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


/*

ldMusicManager provides a collection of audio analysis data for use in music-reactive effects.

Basic values:

- float bass, mids, high:
Bass/mid/high band power in the range of 0-1.

- float volumePowerPre, volumePowerPost:
Sound power before and after volume correction.

- float onsetLargeBeat1, onsetLargeBeat2, onsetBeatWarm, onsetBeatFresh:
Values from 0-1 that detect intense musical events, such as large hits or drops. 'onsetLargeBeat' are
for sudden changes in volume. 'warm' is desgigned to detect large and sustained orchestra hits or
crashes, or the start of a song. 'fresh' tries to detect a sudden change in spectral quality such as
a new instrument beginning to play.

- bool isSilent, isSilent2, isSilent3:
Algorithms to detect when input signal goes silent. isSilent2float provides a 0-1 value that fades out
smoothly when transitioning to silence.

More advanced algorithms are accessed with member class instances. Bass/treb versions of some objects
are available for different reactivitiy. Slow/fast refers to the range of tempos they work best with.

- ldTempoAC tempoACSlower, etc.:
Tempo data based on a spectrum autocorrelation algorithm.
->bpmInstant: beats per minute estimated at the current frame.
->bpmSmooth: beats pre minute value that is more stable, recalculated at the start of each beat.
->freqSmooth: bpmSmooth converted to units of (frame^-1). Useful for advancing a counter each frame.
Phase data is also available to determine when beats hit. A value of 0 represents the beginning of a
beat duration or measure, and will increase to 1 over the time of one beat (expecting the next beat
to hit when it reaches 1). The various values provide different levels of smoothing. x=(phase) can
be used to animate objects progress along a repeating animation (such as rotating). x=(1-phase)
can be used for a signal that resembles a beat impulse (such as flashing).
->float phaseInstant: raw predicted phase (can be chaotic, not recommended).
->float phaseWalker: sawtooth that can pause or reverse in direction to correct, with no large jumps.
->float phaseReactive: sawtooth wave that can reset to 0 mid-phase when an unexpected beat occurs.
->float phaseSmooth: smooth sawtooth wave.

- MusicReactor mrSlowBass, etc.:
Sound intensity detectors with auto normalization and smooth falloff. Range is 0-1. Also provides some
beat detection based on comb filter processing and conveience signals.
->output: recommended value for the particular object, usually equal to signalOutput
->signalOutput: impulse style beat.
->spinOutput: phase style output. Holds at 0 until a large beat, then performs a cycle in a random direction.
->spinOutput4: variant of spinOutput that holds at multiples of 1/4th.
->walkerOutput: phase output that can pause or reverse direction to correct, with no large jumps.

- TempoTracker> tempoTrackerFast, tempoTrackerSlow:
aubio library based beat detection
->output: phase output
->bpm: detected beats pre minute

-HybridAnima hybridAnima:
Algorithm for helping playback of multiple animation sequences and randomly switching between them.
->outputBaseFrameIndex: sequence index from 0-5. This is changed after a sequence comples or on a beat.
->outputTrackPosition: value from 0-1 for the progress within the current animation.

-HybridAutoColor2 hybridAutoColor2:
Algorithm for selecting colors randomly based on music.
->outputColor1, ->outputColor2: 32-bit color values for primary and secondary color
-HybridColorPalette hybridColorPalette:

-HybridColorPalette hybridColorPalette:
Algorithm for using a random music-reactive color space. This is similar to HybridAutoColor2 but instead of
providing 2 direct reference colors as output, you instead supply 3 float values to get a color in the space.
This makes it easier to fade between colors. The input space can be thought of as an abstract color1, color2,
color3, for example if you want 2 different colors and a 3rd color that is between them, use (1,0,0), (0,1,0),
and (0.5,0.5,0). Alternatively, rgb values can be used as input to modify an existing color scheme in a way that
preserves similarity between colors.
->colorize(float& rr...): transforms a value (float triple) into rgb in the new color space. Arguments are
passed by reference and are modified directly by the function.

-HybridFlash hybridFlash:
Algorithm that manages up to 16 color layers, with a limit on how many are visible at the same time. Each
layer will blink, flash, or fade in various ways. To use, loop through your desired drawing layers and
use getColor on each layer index to determine if the layer should be hidden (0) or if not then the color
to draw it with.
->getColor(int index, int max, int limit): returns the value at index. 'max' is the total number of
channels that you want to use, and 'limit' is the number of channels that may visible at the same time.

These classes can be used outside of ldMusicManager if you need to use custom settings for an algorithm.
If instantiating your own copy of these objects, calling the correct update functions every frame
is required.

*/


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
