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
//  ldLdva2AnimationVisualizer.h
//  ldCore
//
//  Created by feldspar on 7/14/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldLdva2AnimationVisualizer__
#define __ldCore__ldLdva2AnimationVisualizer__

#include <QtCore/QObject>
#include <QQmlHelpers>

#include "ldCore/Visualizations/ldVisualizer.h"
#include "ldCore/Visualizations/util/VisualizerHelper/ldAnimationSequence.h"

class ldLdva2AnimationVisualizer : public ldVisualizer
{
public:
    
    ldLdva2AnimationVisualizer();
    ~ldLdva2AnimationVisualizer();

public:

    virtual const char* visualizerName() override { return "Test Visualizer"; }
    virtual float targetFPS() override { return m_fps; }
    bool init();
    virtual void onShouldStart() override;

protected:
    
    virtual void clearBuffer() override;
    virtual void onUpdate(ldSoundData* pSoundData, float delta) override;
    virtual void draw(void) override;

    virtual void onAnimLoadSequence();
    virtual void onAnimBeforeRender();

protected:
    
    // params
    float m_fps;
        
    bool useOldAlg;
    bool doReverse;
    bool doSynchSpeed;
    bool speedAllowSlow;
    bool doWrapOnKeyEnd;
    bool doJumpToKeyStartOnBeat;
    bool jumpBeatMidpoint;
    int jumpBeatCount;    
    
    bool doColorCircle;
    bool doColorRise;
          
    bool doZoom;
    bool doEcho;        

    // internal
    ldAnimationSequenceBezier asb;    
    bool isLoaded;

    float frx = 0;
    int currentFrame;
    int echoFrame;
    int jumpBeatCounter;
    bool rev;
    int ckey;
     
};

#endif /* defined(__ldCore__ldLdva2AnimationVisualizer__) */

