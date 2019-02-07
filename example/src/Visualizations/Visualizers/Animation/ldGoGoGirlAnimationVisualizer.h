//
//  ldGoGoGirlAnimationVisualizer.h
//  LaserdockVisualizer
//
//  Boilerplate code by Eric Brugère on 8/8/16.
//	Animation and changes by MEO 19/11/2016 to 04/12/2016
//  Copyright (c) 2016 Wicked Lasers. All rights reserved.
//
#ifndef ldGoGoGirlAnimationVisualizer_H
#define ldGoGoGirlAnimationVisualizer_H

#include "Visualizations/Visualizers/Animation/ldAnimationVisualizer.h"

class ldGoGoGirlAnimationVisualizer : public ldAnimationVisualizer
{
public:
    //
    
    virtual QString visualizerName() const override { return "Go-Go Girl"; }

    virtual void loadAnimation() override;
    virtual void prepareBeforeRender() override;
};

#endif // ldGoGoGirlAnimationVisualizer_H
