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
    void doEcho(float zoom2);

    // params
    float m_fps = 42;
        
    // animation settings and styles
    bool m_useOldAlg = false; // use old algorithm (loops an animation between key frames) this disregards previous settings
    bool m_doReverse = false; // allow animation to reverse on some types of beats
    bool m_doSynchSpeed = false; // match speed of dancer
    bool m_speedAllowSlow = false; // allow slow motion when using speed matching
    bool m_doWrapOnKeyEnd = false; // when reaching a key end frame, jump to the beginning of that clip
    bool m_doJumpToKeyStartOnBeat = false; // on a loud beat, jump to a random key start frame
    bool m_jumpBeatMidpoint = false; // adjust start frame to match next beat with middle frame
    bool m_usePeakBpm = false; // adjust start frame to match next beat with middle frame
    int m_jumpBeatCount = 1; // 2 = only jump at every 2nd beat
    
    bool m_doColorCircle = false;
    bool m_doColorRise = false;
          
    bool m_doZoom = false; // beat zoom effects
    bool m_doEcho = false;// beat echo effects

    // internal
    ldAnimationSequenceBezier m_asb;
    bool m_isLoaded = false;

    int m_currentFrame = 0;

private:
    float m_totalFrameDelta = 0;

    int m_echoFrame = 0;
    int m_jumpBeatCounter = 0;
    int m_ckey = 0;
};

#endif /* defined(__LaserdockVisualizer__ldAnimationVisualizer__) */

