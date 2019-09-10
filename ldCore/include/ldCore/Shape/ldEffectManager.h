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
//  ldEffectManager.h
//  Laserdock
//
//  Created by rock on 12/28/13.
//  Copyright (c) 2013 Jake Huang. All rights reserved.
//

#ifndef __Laserdock__ldEffectManager__
#define __Laserdock__ldEffectManager__

#include "ldCore/Shape/ldList.h"

#include "ldCore/ldCore_global.h"

class ldShape;
class ldEffect;

class LDCORESHARED_EXPORT ldEffectManager
{
    struct _effect_node
    {
        ldEffect* pEffect;
        ldShape* pTarget;
        bool bPaused;
    };

    
public:
    
    static ldEffectManager* getSharedManager();
    
public:
    
    ldEffectManager();
    
    ~ldEffectManager();
    
    
    /** Adds an effect with a target.
     If the target is already present, then the effect will be added to the existing target.
     If the target is not present, a new instance of this target will be created either paused or not, and the effect will be added to the newly created target.
     When the target is paused, the queued effects won't be 'ticked'.
     */
    void addEffect(ldEffect* pEffect, ldShape* pTarget, bool paused);
    
    /** Removes all effects from all the targets.
     */
    void removeAllEffect();
    
    /** Removes all effects from a certain target.
     All the effects that belongs to the target will be removed.
     */
    void removeAllEffectFromTarget(ldShape* pTarget);
    
    /** Removes an effect given an effect reference.
     */
    void removeEffect(ldEffect *pEffect);

    unsigned int numberOfRunningEffectsInTarget(ldShape *pTarget);
    
    /** Pauses the target: all running effects and newly added effects will be paused.
     */
    void pauseTarget(ldShape *pTarget);

    /** Resumes the target. All queued effects will be resumed.
     */
    void resumeTarget(ldShape *pTarget);
    
    void update(float dt);
    
private:
    
    ldList m_list;
};

#endif /* defined(__Laserdock__ldEffectManager__) */
