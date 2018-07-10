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
//  ldVisualizerHelper.h
//  ldCore
//
//  Created by feldspar on 8/4/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef __ldCore__ldBeatTracker__
#define __ldCore__ldBeatTracker__

#include <vector>

#include <ldCore/ldCore_global.h>

class LDCORESHARED_EXPORT ldBeatTracker {
public:
    ldBeatTracker();

    void add(float f);
    void set(int m_filterstart, int m_filterend);


    float bestbpm() const;
    float bestphase() const;


private:
    std::vector<float> m_history;
    std::vector<float> m_apriori;
    int m_filterstart = 0;
    int m_filterend = 0;

    float m_bestbpm = 0.f;
    float m_bestphase = 0.f;
};

#endif /* defined(__ldCore__ldBeatTracker__) */
