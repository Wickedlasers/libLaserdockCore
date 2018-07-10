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
