//
//  ldAnimationVisualizer.h
//  LaserdockVisualizer
//
//  Created by feldspar on 7/14/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef __LaserdockVisualizer__ldAnimationVisualizer__
#define __LaserdockVisualizer__ldAnimationVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Helpers/Visualizer/ldAnimationSequence.h"

class ldAnimationVisualizer : public ldVisualizer
{
public:
    ldAnimationVisualizer();
    ~ldAnimationVisualizer();

public:
    virtual QString visualizerName() const override { return "Ldva2 Visualizer"; }
    virtual float targetFPS() const override { return m_fps; }
    bool init();

    virtual void onShouldStart() override;

protected:
    virtual void onUpdate(ldSoundData* pSoundData, float delta) override;
    virtual void draw() override;

    virtual void loadAnimation() = 0;
    virtual void prepareBeforeRender() {}

protected:
    // params
    float m_fps = 42;
        
    // animation settings and styles
    bool useOldAlg = false; // use old algorithm (loops an animation between key frames) this disregards previous settings
    bool doReverse = false; // allow animation to reverse on some types of beats
    bool doSynchSpeed = false; // match speed of dancer
    bool speedAllowSlow = false; // allow slow motion when using speed matching
    bool doWrapOnKeyEnd = false; // when reaching a key end frame, jump to the beginning of that clip
    bool doJumpToKeyStartOnBeat = false; // on a loud beat, jump to a random key start frame
    bool jumpBeatMidpoint = false; // adjust start frame to match next beat with middle frame
    int jumpBeatCount = 1; // 2 = only jump at every 2nd beat
    
    bool doColorCircle = false;
    bool doColorRise = false;
          
    bool doZoom = false; // beat zoom effects
    bool doEcho = false;// beat echo effects

    // internal
    ldAnimationSequenceBezier asb;    
    bool isLoaded = false;

    float frx = 0;
    int currentFrame = 0;
    int echoFrame = 0;
    int jumpBeatCounter = 0;
    bool rev = false;
    int ckey = 0;
     
};

#endif /* defined(__LaserdockVisualizer__ldAnimationVisualizer__) */

