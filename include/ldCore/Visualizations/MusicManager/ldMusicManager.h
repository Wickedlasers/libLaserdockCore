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

#include "ldCore/Helpers/Audio/ldAppakaBeat.h"
#include "ldCore/Helpers/Audio/ldAppakGate.h"
#include "ldCore/Helpers/Audio/ldAppakBpmSelector.h"
#include "ldCore/Helpers/Audio/ldAudioBasic.h"
#include "ldCore/Helpers/Audio/ldDurationalStatEstimator.h"
#include "ldCore/Helpers/Audio/ldMusicFeature.h"
#include "ldCore/Helpers/Audio/ldSoundGate.h"
#include "ldCore/Helpers/Audio/ldSilentThree.h"
#include "ldCore/Helpers/Audio/ldSpectrumFrame.h"
#include "ldCore/Helpers/Audio/ldTempoTracker.h"
#include "ldCore/Helpers/Visualizer/ldVisualizerHelper.h"

class ldAppakPeaks;
class ldAppakSpectrum;
class ldSpectrogram;
class ldHybridAnima;
class ldHybridFlash;
class ldHybridAutoColor2;
class ldHybridColorPalette;
class ldTempoAC;

class LDCORESHARED_EXPORT ldMusicManager : public QObject
{
    Q_OBJECT

public:
    // class stuff
    ldMusicManager(QObject* parent);
    ~ldMusicManager();

    // update function
    void updateWith(std::shared_ptr<ldSoundData> psd, float delta);

    // beat variables
    // basic funcs
    float bass() const;
    float mids() const;
    float high() const;
    float volumePowerPre() const;
    float volumePowerPost() const;

    int soundLevel() const; // 1..100

    void setRealSoundLevel(int value);
    int realSoundLevel() const;

    const ldAppakPeaks* peaks() const;

    std::unique_ptr<ldTempoAC> tempoACSlower;
    std::unique_ptr<ldTempoAC> tempoACSlow;
    std::unique_ptr<ldTempoAC> tempoACFast;
    std::unique_ptr<ldTempoAC> tempoACFaster;
    std::unique_ptr<MusicFeature1> musicFeature1;

    std::unique_ptr<ldMusicReactor> mrSlowBass;
    std::unique_ptr<ldMusicReactor> mrFastBass;
    std::unique_ptr<ldMusicReactor> mrSlowTreb;
    std::unique_ptr<ldMusicReactor> mrFastTreb;
    std::unique_ptr<ldMusicReactor> mrVolume;

    std::shared_ptr<ldTempoTracker> tempoTrackerFast;
    std::unique_ptr<ldTempoTracker> tempoTrackerSlow;

    std::unique_ptr<ldAppakaBeat> appakaBeat;
    std::unique_ptr<ldAppakGate> appakaGate;
    std::unique_ptr<ldAppakSpectrum> appakaSpectrum;

    std::unique_ptr<ldAudioBasic> audioBasic ;

    ldSpectrumFrame spectFrame;
    std::unique_ptr<ldSpectAdvanced> spectAdvanced;

    // sound gate
    bool isSilent() const;
    bool isSilent2() const;
    float isSilent2float() const;
    bool isSilent3() const;

	// onset
    float onsetLargeBeat1 = 0.f;
    float onsetLargeBeat2 = 0.f;
    float onsetBeatWarm = 0.f;
    float onsetBeatFresh = 0.f;

	// pitch
//    PitchTracker* pitchTracker = nullptr; // deprecated?
    
    // hybrids;
    std::unique_ptr<ldHybridAnima> hybridAnima;
    std::unique_ptr<ldHybridAutoColor2> hybridAutoColor2;
    std::unique_ptr<ldHybridColorPalette> hybridColorPalette;
    std::unique_ptr<ldHybridFlash> hybridFlash;

    float bestBpm() const;

signals:
    void updated();

private:
    std::shared_ptr<ldSoundData> m_psd;

    std::unique_ptr<ldSpectrogram> spectrogram;
    std::unique_ptr<ldSpectrogram> spectrogram2;
    std::unique_ptr<ldSpectAdvanced> spectAdvanced2;

    bool isFakeSound = false; // kill isSilent (shall be false in production!)
    std::unique_ptr<ldSoundGate> soundGate;
    std::unique_ptr<ldSilentThree> silentThree;

    std::unique_ptr<ldAppakBpmSelector> appakaBpmSelector;
    std::unique_ptr<ldAppakPeaks> m_peaks;

    std::unique_ptr<ldBeatWarm> beatWarm;
    std::unique_ptr<ldBeatFresh> beatFresh;
    float wfr = 0.f;
    ldDurationalStatEstimator dsewfr;

    int m_realSoundLevel = 0;
};

#endif // LDMUSICMANAGER_H
