//
//  ldGoGoGirlAnimationVisualizer.h
//  ldCore
//
//  Boilerplate code by Eric Brugère on 8/8/16.
//	Animation and changes by MEO 19/11/2016 to 04/12/2016
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//
#ifndef ldGoGoGirlAnimationVisualizer_H
#define ldGoGoGirlAnimationVisualizer_H

#include "ldLdva2AnimationVisualizer.h"

class ldGoGoGirlAnimationVisualizer : public ldLdva2AnimationVisualizer
{
public:
    //
    virtual const char* getInternalName() override { return __FILE__; }
    virtual const char* visualizerName() override { return "Go-Go Girl"; }
    virtual QString previewFilename() const override { return "Animation-Go-Go-Girl.png"; }

    virtual void onAnimLoadSequence() override;
    virtual void onAnimBeforeRender() override;
};

#endif // ldGoGoGirlAnimationVisualizer_H
