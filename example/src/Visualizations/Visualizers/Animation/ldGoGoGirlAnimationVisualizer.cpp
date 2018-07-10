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
