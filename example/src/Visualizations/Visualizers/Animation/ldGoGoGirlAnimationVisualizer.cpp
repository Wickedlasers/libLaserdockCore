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
    m_asb.load2(ldCore::instance()->resourceDir() + "/ldva2/" + filename + ".ldva2");


    // apply animation settings and styles
    m_doSynchSpeed = true;
    m_speedAllowSlow = true;
    m_doReverse = false;
    m_doWrapOnKeyEnd = false;
    m_doJumpToKeyStartOnBeat = true;
    m_jumpBeatMidpoint = false; 
    m_jumpBeatCount = 2; 
    m_useOldAlg = false;
    m_doColorCircle = true;
    m_doZoom = false;
    m_doEcho = false;


    /// save to ldva2
    // if we didnt load form a binary, then save it instead
    if (!useLDVAFile) m_asb.save2("./" + filename + ".ldva2"); // (this puts it in current directory)

}

void ldGoGoGirlAnimationVisualizer::prepareBeforeRender() {
    // do any custom render params, transforms, or filters here
}
