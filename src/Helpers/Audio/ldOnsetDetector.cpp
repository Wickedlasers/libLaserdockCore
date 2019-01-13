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
//  ldOnsetDetector.cpp
//  ldCore
//
//  Created by feldspar on 3/24/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//

/*!

  OnsetDetector class.
  Call process() to feed in new data.
  Read member variable "output" to get result.

  Output value is from 0 to 1.  No onset returns zero.  When an onset is detected, it will raise the output
  to a value between 0.5 and 1, depending on the intensity of the onset.  Optionally, this value may decay
  slowly or instantly return to 0 next frame.

  Use the constructor to specify what type of onset detector.
  algorithm : the algorithm type passed to aubio.  the only values that seem to work well are
        "phase";
        "specdiff";
        "specflux";

  gapTime : the minimum time interval in seconds between onsets.

  decayTime : set this to 0 to get output as a single pulse wave.  values greater than 0 will create a sawtooth
  type wave with maximum base length in seconds determined by decayTime.

  threshold : parameter passed to aubio

*/

#include "ldCore/Helpers/Audio/ldOnsetDetector.h"

#include <QtCore/QDebug>


static const uint_t samplerate = 44100;
static const uint_t win_size = AUDIO_BLOCK_SIZE/2; // /2 is for convert to mono

OnsetDetector::OnsetDetector(char* _algorithm, float _gapTime, float _decayTime, float _threshold)
    : algorithm(_algorithm) ,
            gapTime (_gapTime),
            decayTime(_decayTime),
            threshold(_threshold) {

    output = 0;
    overdriveSkip = -1;

    // create onset object
    hop_size = win_size;
    o = new_aubio_onset(algorithm, win_size, hop_size, samplerate);
    aubio_onset_set_minioi_s(o, gapTime);
    aubio_onset_set_threshold(o, threshold);

}


float OnsetDetector::process(ldSoundData* pSoundData) {

    // decay
    if (decayTime > AUDIO_UPDATE_DELTA_S) {
        output -= AUDIO_UPDATE_DELTA_S / decayTime;
        output = std::max(0.f, output);
    } else output = 0;

    // skip for overdrive factor, we are fixed 30fps
    overdriveSkip++;
    overdriveSkip %= AUDIO_OVERDRIVE_FACTOR;    
    //if (overdriveSkip) return output;

    // create some vectors
    fvec_t * in = new_fvec (hop_size); // input audio buffer
    fvec_t * out = new_fvec (1);

    // put some fresh data in input vector
    for (int i = 0; i < hop_size; i++) {
        float samp = pSoundData->GetWaveformL(i) + pSoundData->GetWaveformR(i);
        fvec_set_sample(in, 8*samp, i);
    }

    // execute onset
    aubio_onset_do(o,in,out);
    //qDebug() << fvec_get_sample(out, 0) << ", " << out->data[0];

    // get output
    output = std::min(std::max(output, fvec_get_sample(out, 0)/2), 1.f);
    //output = out->data[0];

    // clean up memory
    del_fvec(in);
    del_fvec(out);

    return output;
}
