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
//  ldEffectManager.cpp
//  Laserdock
//
//  Created by rock on 12/28/13.
//  Copyright (c) 2013 Jake Huang. All rights reserved.
//

#include "ldCore/Shape/ldShape.h"
#include "ldCore/Shape/ldEffect.h"
#include <stdlib.h>

ldEffectManager* g_pSharedManager = NULL;

/*!
  * \class ldEffectManager
  * \inmodule Laserdock Project
  * \brief The ldEffectManager is a manager of ldEffects.
  */

/*!
 * \brief Returns a shared global EffectManager.
 */
ldEffectManager* ldEffectManager::getSharedManager()
{
    if (!g_pSharedManager)
    {
        g_pSharedManager = new ldEffectManager();
    }
    
    return g_pSharedManager;
}

/*!
 * \brief ldEffectManager::ldEffectManager
 */
ldEffectManager::ldEffectManager()
{
    
}

/*!
 * \brief ldEffectManager::~ldEffectManager
 */
ldEffectManager::~ldEffectManager()
{
    removeAllEffect();
}

/*!
 * \brief Adds a \a pEffect with a \a pTarget shape.
 * If the target is already present, then the effect will be added to the existing target.
 * If the target is not present, a new instance of this target will be created either paused or not, and the effect will be added to the newly created target.
 * When the target is \a paused, the queued effects won't be 'ticked'.
 */
void ldEffectManager::addEffect(ldEffect *pEffect, ldShape *pTarget, bool /*paused*/)
{
    if (!pEffect || !pTarget) return;
    
    _effect_node* newnode = (_effect_node*)malloc(sizeof(_effect_node));
    
    newnode->pEffect = pEffect;
    newnode->pTarget = pTarget;
    newnode->bPaused = false;
    
    pEffect->startWithTarget(pTarget);
    
    m_list.add(newnode);
}

/*!
 * \brief Removes all effects from all the targets.
 */
void ldEffectManager::removeAllEffect()
{
    m_list.begin();
    
    while (m_list.isNext())
    {
        free(m_list.moveNext());
    }
    
    m_list.removeAll();
}

/*!
 * \brief Removes all effects from a certain \a pTarget.
 * All the effects that belongs to the target will be removed.
 */
void ldEffectManager::removeAllEffectFromTarget(ldShape* pTarget)
{
    if (!pTarget) return;
    
    _effect_node* node;
    
    for (int i = 0; i < m_list.length(); i++)
    {
        node = (_effect_node*)m_list.getItemByIndex(i);
        
        if (node->pTarget == pTarget)
        {
            m_list.removeByIndex(i);
            
            free(node);
            
            i--;
        }
    }
}

/*!
 * \brief Removes an effect given an \a pEffect reference.
 */
void ldEffectManager::removeEffect(ldEffect *pEffect)
{
    _effect_node* node;
    
    for (int i = 0; i < m_list.length(); i++)
    {
        node = (_effect_node*)m_list.getItemByIndex(i);
        
        if (node->pEffect == pEffect)
        {
            m_list.removeByIndex(i);
            
            free(node);
            
            return;
        }
    }
}

/*!
 * \brief Gets number of running effects on pTarget;
 */
unsigned int ldEffectManager::numberOfRunningEffectsInTarget(ldShape * /*pTarget*/)
{
    return 0;
}

/*!
 * \brief  Pauses the \a pTarget: all running effects and newly added effects will be paused.
 */
void ldEffectManager::pauseTarget(ldShape *pTarget)
{
    if (!pTarget) return;

    void* object;
    LDLIST_FOREACH((&m_list), object)
    {
        _effect_node* node = (_effect_node*) object;
        
        if (node->pTarget == pTarget) node->bPaused = true;
    }
}

/*!
 * \brief Resumes the \a pTarget. All queued effects will be resumed.
 */
void ldEffectManager::resumeTarget(ldShape *pTarget)
{
    if (!pTarget) return;

    void* object;
    LDLIST_FOREACH((&m_list), object)
    {
        _effect_node* node = (_effect_node*) object;
        
        if (node->pTarget == pTarget) node->bPaused = false;
    }
}

/*!
 * \brief ldEffectManager::update
 * The param \a dt is a delay time.
 */
void ldEffectManager::update(float dt)
{
    _effect_node* node;
    
    void* object;
    LDLIST_FOREACH((&m_list), object)
    {
        node = (_effect_node*)object;
        
        if (!node->bPaused)
        {
            node->pEffect->step(dt);
        }
    }
    
    for (int i = 0; i < m_list.length(); i++)
    {
        node = (_effect_node*)m_list.getItemByIndex(i);
        
        if (node->pEffect->isDone())
        {
            node->pEffect->stop();
            
            m_list.removeByIndex(i);
            
            free(node);
            
            i--;
        }
    }
}

