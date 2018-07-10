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
//  ldGoGoGirlAnimationVisualizer.cpp
//  ldCore
//
//  Boilerplate code by Eric Brugère on 10/24/16.
//	Animation and changes by MEO 19/11/2016 to 04/12/2016
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldGoGoGirlAnimationVisualizer.h"

#include "ldCore/ldCore.h"

void ldGoGoGirlAnimationVisualizer::onAnimLoadSequence() {

    ///  this function should load animation into asb object
    ///  and set animation styles

    // switch- if we load from binary, or load from another format and convert
    bool useLDVAFile = true;
    QString filename = "gogo";
    asb.load2(QString(ldCore::instance()->resourceDir() + "/ldva2/" + filename + ".ldva2").toUtf8().constData());


    // apply animation settings and styles
    doSynchSpeed = true;// match speed of dancer
    speedAllowSlow = true; // allow slow motion when using speed matching
    doReverse = false;// allow animation to reverse on some types of beats
    doWrapOnKeyEnd = false;// when reaching a key end frame, jump to the beginning of that clip
    doJumpToKeyStartOnBeat = true;// on a loud beat, jump to a random key start frame
    jumpBeatMidpoint = false; // adjust start frame to match next beat with middle frame
    jumpBeatCount = 2; // 2 = only jump at every 2nd beat
    useOldAlg = false;// use old algorithm (loops an animation between key frames) this disregards previous settings
    doColorCircle = true;
    doZoom = false;// beat zoom and echo effects
    doEcho = false;


    /// save to ldva2
    // if we didnt load form a binary, then save it instead
    if (!useLDVAFile) asb.save2(QString("./" + filename + ".ldva2").toUtf8().constData()); // (this puts it in current directory)

}

void ldGoGoGirlAnimationVisualizer::onAnimBeforeRender() {
    // do any custom render params, transforms, or filters here
}
