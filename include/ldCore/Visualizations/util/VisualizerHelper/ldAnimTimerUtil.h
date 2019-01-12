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
//  ldAnimTimerUtil.h
//
//  Created by Eric Brugère on 2/10/15.
//  Copyright (c) 2015 Wicked Lasers. All rights reserved.
//
#ifndef LDANIMTIMERUTIL_H
#define LDANIMTIMERUTIL_H

#include <ldCore/ldCore_global.h>

class LDCORESHARED_EXPORT ldAnimTimerUtil
{
public:
    struct AnimTimerStruct {
        bool isAvailable;
        float elapsed;
        float duration;
        float gradient;
    };

    typedef std::vector<std::vector<AnimTimerStruct> > anim_timer_struct_vec_t;

    ldAnimTimerUtil();
    ~ldAnimTimerUtil();

    void initWithAnimsAndStepFamilies(int maxAnims, int maxFamilies);
    void reinit();
    void update(float delta);

    void startTimer(int animId, int stepFamilyId, float duration);
    bool isAnimAvailable(int animId, int stepFamilyId);
    float gradientForAnimAndFamily(int animId, int stepFamilyId);
    float durationForAnimAndFamily(int animId, int stepFamilyId);

private:
    void initAnimForFamily(int animId, int stepFamilyId);

    int m_maxAnims;
    int m_maxFamilies;

    anim_timer_struct_vec_t m_timers;
};

#endif // LDANIMTIMERUTIL_H
