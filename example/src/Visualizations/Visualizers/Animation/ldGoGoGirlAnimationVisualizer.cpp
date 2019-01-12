//
//  ldGoGoGirlAnimationVisualizer.cpp
//  LaserdockVisualizer
//
//  Boilerplate code by Eric Brugère on 10/24/16.
//	Animation and changes by MEO 19/11/2016 to 04/12/2016
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//

#include "ldGoGoGirlAnimationVisualizer.h"

#include "ldCore/ldCore.h"

void ldGoGoGirlAnimationVisualizer::loadAnimation() {

    ///  this function should load animation into asb object
    ///  and set animation styles

    // switch- if we load from binary, or load from another format and convert
    bool useLDVAFile = true;
    QString filename = "gogo";
    asb.load2(ldCore::instance()->resourceDir() + "/ldva2/" + filename + ".ldva2");


    // apply animation settings and styles
    doSynchSpeed = true;
    speedAllowSlow = true;
    doReverse = false;
    doWrapOnKeyEnd = false;
    doJumpToKeyStartOnBeat = true;
    jumpBeatMidpoint = false; 
    jumpBeatCount = 2; 
    useOldAlg = false;
    doColorCircle = true;
    doZoom = false;
    doEcho = false;


    /// save to ldva2
    // if we didnt load form a binary, then save it instead
    if (!useLDVAFile) asb.save2("./" + filename + ".ldva2"); // (this puts it in current directory)

}

void ldGoGoGirlAnimationVisualizer::prepareBeforeRender() {
    // do any custom render params, transforms, or filters here
}
