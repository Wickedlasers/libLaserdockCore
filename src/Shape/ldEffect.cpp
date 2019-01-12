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
//  ldTextEffect.cpp
//  Laserdock
//
//  Created by rock on 12/27/13.
//  Copyright (c) 2013 Jake Huang. All rights reserved.
//

#include "ldCore/Shape/ldEffect.h"
#include "ldCore/Shape/ldEffectManager.h"
#include <stdarg.h>

/*!
 * \class EmptyEffect
 * \brief Empty effect for making a CCSequence or CCSpawn when only adding one action to it.
 */
class EmptyEffect : public ldFiniteTimeEffect
{
public:
    static EmptyEffect* create();

    virtual void update(float time);
    virtual void step(float dt);
};

EmptyEffect* EmptyEffect::create()
{
    EmptyEffect* pRet = new EmptyEffect();

    return pRet;
}

void EmptyEffect::update(float /*time*/)
{
    // unsed param time
    
    return;
}

void EmptyEffect::step(float /*dt*/)
{
    // unsed param dt
    
    return;
}

//
// ldEffect
//
/*!
 * \class ldEffect
 * \brief The ldEffect class
 * \inmodule Laserdock Project
 */
/*!
 * \fn ldEffect::ldEffect()
 * \brief ldEffect::ldEffect
 */
ldEffect::ldEffect()
:m_pTarget(NULL)
{
    
}

/*!
 * \brief ldEffect::~ldEffect
 */
ldEffect::~ldEffect()
{
    
}

/*!
 * \brief Checks whether this effect is running.
 * Returns \c true if running, \c false otherwise.
 */
bool ldEffect::isDone()
{
    return true;
}

/*!
 * \brief Called before the effect start. It will also set the target.
 * The param \a pTarget
 */
void ldEffect::startWithTarget(ldShape *pTarget)
{
    if (!pTarget || m_pTarget) return;
    
    m_pTarget = pTarget;
    
//    ldEffectManager* gManager = ldEffectManager::getSharedManager();
//    
//    gManager->addEffect(this, pTarget, pTarget->isRunning());
}

/*!
 * \brief called after the effect has finished. It will set the 'target' to NULL.
 * You should never call this method manually. Instead, use "target->stopEffect(pEffect);"
 */
void ldEffect::stop()
{
    m_pTarget = NULL;
    
//    ldEffectManager* gManager = ldEffectManager::getSharedManager();
//    
//    gManager->removeEffect(this);
}

/*!
 * \brief Called on each frame with it's delta time.
 * The param \a dt is a delta time.
 */
void ldEffect::step(float /*dt*/)
{
    // Effect step(). Overide me.
}

/*!
 * \brief Called once per frame.
 * The param \a time is a delay time.
 */
void ldEffect::update(float /*time*/)
{
    // Effect update(). Overide me.
}

//
// IntervalEffect
//
bool ldEffectInterval::initWithDuration(float d)
{
    m_fDuration = d;
    
    // prevent division by 0
    // This comparison could be in step:, but it might decrease the performance
    // by 3% in heavy based action games.
    if (m_fDuration == 0)
    {
        m_fDuration = FLT_EPSILON;
    }
    
    m_elapsed = 0;
    m_bFirstTick = true;
    
    return true;
}

bool ldEffectInterval::isDone(void)
{
    return m_elapsed >= m_fDuration;
}

void ldEffectInterval::step(float dt)
{
    if (m_bFirstTick)
    {
        m_bFirstTick = false;
        m_elapsed = 0;
    }
    else
    {
        m_elapsed += dt;
    }
    
    this->update(MAX (0,                                  // needed for rewind. elapsed could be negative
                      MIN(1, m_elapsed /
                          MAX(m_fDuration, FLT_EPSILON)   // division by 0
                          )
                      )
                 );
}

void ldEffectInterval::startWithTarget(ldShape *pTarget)
{
    ldFiniteTimeEffect::startWithTarget(pTarget);
    m_elapsed = 0.0f;
    m_bFirstTick = true;
}


//
// DelayTime
//
ldDelayTime* ldDelayTime::create(float d)
{
    ldDelayTime* pRet = new ldDelayTime();
    
    pRet->initWithDuration(d);
    
    return pRet;
}

void ldDelayTime::update(float /*time*/)
{
    // unsed parameter time.
    
    return;
}

//
// Sequence
//
ldSequence* ldSequence::createWithTwoActions(ldFiniteTimeEffect *pEffectOne, ldFiniteTimeEffect *pEffectTwo)
{
    ldSequence *pRet = new ldSequence();
    pRet->initWithTwoEffects(pEffectOne, pEffectTwo);
    return pRet;
}

ldSequence* ldSequence::create(ldFiniteTimeEffect *pEffect, ...)
{
    va_list params;
    va_start(params, pEffect);
    
    ldSequence *pRet = ldSequence::createWithVariableList(pEffect, params);
    
    va_end(params);
    
    return pRet;
}

ldSequence* ldSequence::createWithVariableList(ldFiniteTimeEffect *pEffect1, va_list args)
{
    ldFiniteTimeEffect* pNow;
    ldFiniteTimeEffect* pPrev = pEffect1;
    bool bOneEffect = true;
    
    while (pEffect1)
    {
        pNow = va_arg(args, ldFiniteTimeEffect*);
        
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
            bOneEffect = false;
        }
        else
        {
            // If only one action is added to CCSequence, make up a CCSequence by adding a simplest finite time action.
            if (bOneEffect)
            {
                pPrev = createWithTwoActions(pPrev, EmptyEffect::create());
            }
            break;

        }
    }
    
    return ((ldSequence*)pPrev);
}

bool ldSequence::initWithTwoEffects(ldFiniteTimeEffect *pEffectOne, ldFiniteTimeEffect *pEffectTwo)
{
    if (!pEffectOne || !pEffectTwo) return false;
    
    float d = pEffectOne->getDuration() + pEffectTwo->getDuration();
    
    ldEffectInterval::initWithDuration(d);
    
    m_pEffects[0] = pEffectOne;
    m_pEffects[1] = pEffectTwo;
    
    return true;
}

ldSequence::~ldSequence()
{
    delete m_pEffects[0];
    delete m_pEffects[1];
}

void ldSequence::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    m_split = m_pEffects[0]->getDuration() / m_fDuration;
    m_last = -1;
}

void ldSequence::stop()
{
    // Issue #1305
    if( m_last != - 1)
    {
        m_pEffects[m_last]->stop();
    }
    
    ldEffectInterval::stop();
}

void ldSequence::update(float t)
{
    int found = 0;
    float new_t = 0.0f;
    
    if( t < m_split ) {
        // action[0]
        found = 0;
        if( m_split != 0 )
            new_t = t / m_split;
        else
            new_t = 1;
        
    } else {
        // action[1]
        found = 1;
        if ( m_split == 1 )
            new_t = 1;
        else
            new_t = (t-m_split) / (1 - m_split );
    }
    
    if ( found==1 ) {
        
        if( m_last == -1 ) {
            // action[0] was skipped, execute it.
            m_pEffects[0]->startWithTarget(m_pTarget);
            m_pEffects[0]->update(1.0f);
            m_pEffects[0]->stop();
        }
        else if( m_last == 0 )
        {
            // switching to action 1. stop action 0.
            m_pEffects[0]->update(1.0f);
            m_pEffects[0]->stop();
        }
    }
	else if(found==0 && m_last==1 )
	{
		// Reverse mode ?
		// XXX: Bug. this case doesn't contemplate when _last==-1, found=0 and in "reverse mode"
		// since it will require a hack to know if an action is on reverse mode or not.
		// "step" should be overriden, and the "reverseMode" value propagated to inner Sequences.
		m_pEffects[1]->update(0);
		m_pEffects[1]->stop();
	}
    // Last action found and it is done.
    if( found == m_last && m_pEffects[found]->isDone() )
    {
        return;
    }
    
    // Last action found and it is done
    if( found != m_last )
    {
        m_pEffects[found]->startWithTarget(m_pTarget);
    }
    
    m_pEffects[found]->update(new_t);
    m_last = found;
}


//
// Spawn
//
ldSpawn* ldSpawn::create(ldFiniteTimeEffect *pEffect1, ...)
{
    va_list params;
    va_start(params, pEffect1);
    
    ldSpawn *pRet = ldSpawn::createWithVariableList(pEffect1, params);
    
    va_end(params);
    
    return pRet;
}

ldSpawn* ldSpawn::createWithVariableList(ldFiniteTimeEffect *pEffect1, va_list args)
{
    ldFiniteTimeEffect *pNow;
    ldFiniteTimeEffect *pPrev = pEffect1;
    bool bOneAction = true;
    
    while (pEffect1)
    {
        pNow = va_arg(args, ldFiniteTimeEffect*);
        if (pNow)
        {
            pPrev = createWithTwoActions(pPrev, pNow);
            bOneAction = false;
        }
        else
        {
            // If only one action is added to CCSpawn, make up a CCSpawn by adding a simplest finite time action.
            if (bOneAction)
            {
                pPrev = createWithTwoActions(pPrev, EmptyEffect::create());
            }
            break;
        }
    }
    
    return ((ldSpawn*)pPrev);

}

ldSpawn* ldSpawn::createWithTwoActions(ldFiniteTimeEffect *pEffect1, ldFiniteTimeEffect *pEffect2)
{
    ldSpawn *pSpawn = new ldSpawn();
    pSpawn->initWithTwoActions(pEffect1, pEffect2);
    
    return pSpawn;
}

bool ldSpawn:: initWithTwoActions(ldFiniteTimeEffect *pEffect1, ldFiniteTimeEffect *pEffect2)
{
    if (!pEffect1 || !pEffect2) return false;
    
    bool bRet = false;
    
    float d1 = pEffect1->getDuration();
    float d2 = pEffect2->getDuration();
    
    if (ldEffectInterval::initWithDuration(MAX(d1, d2)))
    {
        m_pOne = pEffect1;
        m_pTwo = pEffect2;
        
        if (d1 > d2)
        {
            m_pTwo = ldSequence::createWithTwoActions(pEffect2, ldDelayTime::create(d1 - d2));
        }
        else if (d1 < d2)
        {
            m_pOne = ldSequence::createWithTwoActions(pEffect1, ldDelayTime::create(d2 - d1));
        }
        
        bRet = true;
    }
    
    
    return bRet;
}

ldSpawn::~ldSpawn()
{
    delete m_pOne;
    delete m_pTwo;
}

void ldSpawn::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    m_pOne->startWithTarget(pTarget);
    m_pTwo->startWithTarget(pTarget);
}

void ldSpawn::stop()
{
    m_pOne->stop();
    m_pTwo->stop();
}

void ldSpawn::update(float time)
{
    if (m_pOne)
    {
        m_pOne->update(time);
    }
    if (m_pTwo)
    {
        m_pTwo->update(time);
    }
}



//
// Repeat
//
ldRepeat* ldRepeat::create(ldFiniteTimeEffect *pEffect, unsigned int times)
{
    ldRepeat* pRepeat = new ldRepeat();
    pRepeat->initWithEffect(pEffect, times);
    
    return pRepeat;
}

bool ldRepeat::initWithEffect(ldFiniteTimeEffect *pEffect, unsigned int times)
{
    float d = pEffect->getDuration() * times;
    
    if (ldEffectInterval::initWithDuration(d))
    {
        m_uTimes = times;
        m_pInnerEffect = pEffect;
        
        m_bActionInstant = false; //dynamic_cast<ldEffectInstant*>(pEffect) ? true : false;
        //an instant action needs to be executed one time less in the update method since it uses startWithTarget to execute the action
        if (m_bActionInstant)
        {
            m_uTimes -=1;
        }
        m_uTotal = 0;
        
        return true;
    }
    
    return false;
}

ldRepeat::~ldRepeat(void)
{
    delete m_pInnerEffect;
    m_pInnerEffect = NULL;
}

void ldRepeat::startWithTarget(ldShape *pTarget)
{
    m_uTotal = 0;
    m_fNextDt = m_pInnerEffect->getDuration()/m_fDuration;
    ldEffectInterval::startWithTarget(pTarget);
    m_pInnerEffect->startWithTarget(pTarget);
}

void ldRepeat::stop(void)
{
    m_pInnerEffect->stop();
    ldEffectInterval::stop();
}

// issue #80. Instead of hooking step:, hook update: since it can be called by any
// container action like ldRepeat, CCSequence, CCEase, etc..
void ldRepeat::update(float dt)
{
    if (dt >= m_fNextDt)
    {
        while (dt > m_fNextDt && m_uTotal < m_uTimes)
        {
            
            m_pInnerEffect->update(1.0f);
            m_uTotal++;
            
            m_pInnerEffect->stop();
            m_pInnerEffect->startWithTarget(m_pTarget);
            m_fNextDt += m_pInnerEffect->getDuration()/m_fDuration;
        }
        
        // fix for issue #1288, incorrect end value of repeat
        if(dt >= 1.0f && m_uTotal < m_uTimes)
        {
            m_uTotal++;
        }
        
        // don't set an instant action back or update it, it has no use because it has no duration
        if (!m_bActionInstant)
        {
            if (m_uTotal == m_uTimes)
            {
                m_pInnerEffect->update(1);
                m_pInnerEffect->stop();
            }
            else
            {
                // issue #390 prevent jerk, use right update
                m_pInnerEffect->update(dt - (m_fNextDt - m_pInnerEffect->getDuration()/m_fDuration));
            }
        }
    }
    else
    {
        m_pInnerEffect->update(fmodf(dt * m_uTimes,1.0f));
    }
}

bool ldRepeat::isDone(void)
{
    return m_uTotal == m_uTimes;
}


//
// RepeatForever
//
ldRepeatForever::~ldRepeatForever()
{
    delete m_pInnerEffect;
    m_pInnerEffect = NULL;
}

ldRepeatForever *ldRepeatForever::create(ldEffectInterval *pEffect)
{
    ldRepeatForever *pRet = new ldRepeatForever();
    if(pRet->initWithEffect(pEffect))
    {
        return pRet;
    }
    delete pRet;
    return NULL;
}

bool ldRepeatForever::initWithEffect(ldEffectInterval *pEffect)
{
    if (pEffect == NULL) return false;
    
    m_pInnerEffect = pEffect;

    return true;
}

void ldRepeatForever::startWithTarget(ldShape* pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    m_pInnerEffect->startWithTarget(pTarget);
}

void ldRepeatForever::step(float dt)
{
    m_pInnerEffect->step(dt);
    if (m_pInnerEffect->isDone())
    {
        float diff = m_pInnerEffect->getElapsed() - m_pInnerEffect->getDuration();
        m_pInnerEffect->startWithTarget(m_pTarget);
        // to prevent jerk. issue #390, 1247
        m_pInnerEffect->step(0.0f);
        m_pInnerEffect->step(diff);
    }
}

bool ldRepeatForever::isDone()
{
    return false;
}


//
// MoveBy
//
ldMoveBy* ldMoveBy::create(float duration, const CCPoint &deltaPosition)
{
    ldMoveBy* pRet = new ldMoveBy();
    pRet->initWithDuration(duration, deltaPosition);
    return pRet;
}

bool ldMoveBy::initWithDuration(float duration, const CCPoint &deltaPosition)
{
    if (ldMoveBy::ldEffectInterval::initWithDuration(duration))
    {
        m_positionDelta = deltaPosition;
        return true;
    }
    return false;
}

void ldMoveBy::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    m_previousPosition = m_startPosition = pTarget->getPosition();
}


void ldMoveBy::update(float t)
{
    if (m_pTarget)
    {
//#if CC_ENABLE_STACKABLE_ACTIONS
        CCPoint currentPos = m_pTarget->getPosition();
        CCPoint diff = ccpSub(currentPos, m_previousPosition);
        m_startPosition = ccpAdd( m_startPosition, diff);
        CCPoint newPos =  ccpAdd( m_startPosition, ccpMult(m_positionDelta, t) );
        m_pTarget->setPosition(newPos);
        m_previousPosition = newPos;
//#else
//        m_pTarget->setPosition(ccpAdd( m_startPosition, ccpMult(m_positionDelta, t)));
//#endif // CC_ENABLE_STACKABLE_ACTIONS
    }
}

//
// MoveTo
//
ldMoveTo* ldMoveTo::create(float duration, const CCPoint &position)
{
    ldMoveTo* pRet = new ldMoveTo();
    pRet->initWithDuration(duration, position);
    return pRet;
}

bool ldMoveTo::initWithDuration(float duration, const CCPoint &position)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_endPosition = position;
        return true;
    }
    return false;
}

void ldMoveTo::startWithTarget(ldShape *pTarget)
{
    ldMoveBy::startWithTarget(pTarget);
    m_positionDelta = ccpSub( m_endPosition, pTarget->getPosition() );
}

//
// ScaleTo
//
ldScaleTo* ldScaleTo::create(float duration, float s)
{
    ldScaleTo* pRet = new ldScaleTo();
    
    pRet->initWithDuration(duration, s);
    
    return pRet;
}

ldScaleTo* ldScaleTo::create(float duration, float sx, float sy)
{
    ldScaleTo* pRet = new ldScaleTo();
    
    pRet->initWithDuration(duration, sx, sy);
    
    return pRet;
}

bool ldScaleTo::initWithDuration(float duration, float s)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_fEndScaleX = s;
        m_fEndScaleY = s;
        
        return  true;
    }
    
    return false;
}

bool ldScaleTo::initWithDuration(float duration, float sx, float sy)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_fEndScaleX = sx;
        m_fEndScaleY = sy;
        
        return  true;
    }
    
    return false;
}

void ldScaleTo::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    
    m_fStartScaleX = pTarget->getScaleX();
    m_fStartScaleY = pTarget->getScaleY();
    m_fDeltaX = m_fEndScaleX - m_fStartScaleX;
    m_fDeltaY = m_fEndScaleY - m_fStartScaleY;
}

void ldScaleTo::update(float time)
{
    if (m_pTarget)
    {
        m_pTarget->setScaleX(m_fStartScaleX + m_fDeltaX * time);
        m_pTarget->setScaleY(m_fStartScaleY + m_fDeltaY * time);
    }
}

//
// ScaleBy
//
ldScaleBy* ldScaleBy::create(float duration, float s)
{
    ldScaleBy* pRet = new ldScaleBy();
    
    pRet->initWithDuration(duration, s);
    
    return pRet;
}

ldScaleBy* ldScaleBy::create(float duration, float sx, float sy)
{
    ldScaleBy* pRet = new ldScaleBy();
    
    pRet->initWithDuration(duration, sx, sy);
    
    return pRet;
}

void ldScaleBy::startWithTarget(ldShape *pTarget)
{
    ldScaleTo::startWithTarget(pTarget);
    
    m_fDeltaX = m_fStartScaleX * m_fEndScaleX - m_fStartScaleX;
    m_fDeltaY = m_fStartScaleY * m_fEndScaleY - m_fStartScaleY;

}

//
// RotateTo
//
ldRotateTo* ldRotateTo::create(float duration, float fDeltaAngle)
{
    ldRotateTo* pRet = new ldRotateTo();
    pRet->initWithDuration(duration, fDeltaAngle);
    return pRet;
}

bool ldRotateTo::initWithDuration(float duration, float fDeltaAngle)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_fDstAngleX = m_fDstAngleY = fDeltaAngle;
        
        return true;
    }
    
    return false;
}

ldRotateTo* ldRotateTo::create(float duration, float fDeltaAngleX, float fDeltaAngleY)
{
    ldRotateTo* pRet = new ldRotateTo();
    
    pRet->initWithDuration(duration, fDeltaAngleX, fDeltaAngleY);
    
    return pRet;
}

bool ldRotateTo::initWithDuration(float duration, float fDeltaAngleX, float fDeltaAngleY)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_fDiffAngleX = fDeltaAngleX;
        m_fDiffAngleY = fDeltaAngleY;
        
        return true;
    }
    
    return false;
}

void ldRotateTo::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    
    // Calculate X
    m_fStartAngleX = pTarget->getRotationX();
    if (m_fStartAngleX > 0)
    {
        m_fStartAngleX = fmodf(m_fStartAngleX, 360.0f);
    }
    else
    {
        m_fStartAngleX = fmodf(m_fStartAngleX, -360.0f);
    }
    
    m_fDiffAngleX = m_fDstAngleX - m_fStartAngleX;
    if (m_fDiffAngleX > 180)
    {
        m_fDiffAngleX -= 360;
    }
    if (m_fDiffAngleX < -180)
    {
        m_fDiffAngleX += 360;
    }
    
    //Calculate Y: It's duplicated from calculating X since the rotation wrap should be the same
    m_fStartAngleY = m_pTarget->getRotationY();
    
    if (m_fStartAngleY > 0)
    {
        m_fStartAngleY = fmodf(m_fStartAngleY, 360.0f);
    }
    else
    {
        m_fStartAngleY = fmodf(m_fStartAngleY, -360.0f);
    }
    
    m_fDiffAngleY = m_fDstAngleY - m_fStartAngleY;
    if (m_fDiffAngleY > 180)
    {
        m_fDiffAngleY -= 360;
    }
    
    if (m_fDiffAngleY < -180)
    {
        m_fDiffAngleY += 360;
    }
}

void ldRotateTo::update(float time)
{
    if (m_pTarget)
    {
        m_pTarget->setRotationX(m_fStartAngleX + m_fDiffAngleX * time);
        m_pTarget->setRotationY(m_fStartAngleY + m_fDiffAngleY * time);
    }
}

//
// RotateBy
//
ldRotateBy* ldRotateBy::create(float fDuration, float fDeltaAngle)
{
    ldRotateBy* pRet = new ldRotateBy();
    pRet->initWithDuration(fDuration, fDeltaAngle);
    return pRet;
}

bool ldRotateBy::initWithDuration(float fDuration, float fDeltaAngle)
{
    if (ldEffectInterval::initWithDuration(fDuration))
    {
        m_fAngleX = m_fAngleY = fDeltaAngle;
        return true;
    }
    return false;
}

ldRotateBy* ldRotateBy::create(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    ldRotateBy* pRet = new ldRotateBy();
    pRet->initWithDuration(fDuration, fDeltaAngleX, fDeltaAngleY);
    return pRet;
}

bool ldRotateBy::initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY)
{
    if (ldEffectInterval::initWithDuration(fDuration))
    {
        m_fAngleX = fDeltaAngleX;
        m_fAngleY = fDeltaAngleY;
        return true;
    }
    return false;
}

void ldRotateBy::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    m_fStartAngleX = pTarget->getRotationX();
    m_fStartAngleY = pTarget->getRotationY();
}

void ldRotateBy::update(float time)
{
    // XXX: shall I add % 360
    if (m_pTarget)
    {
        m_pTarget->setRotationX(m_fStartAngleX + m_fAngleX * time);
        m_pTarget->setRotationY(m_fStartAngleY + m_fAngleY * time);
    }

}

//
// WipeTo
//
ldWipeTo* ldWipeTo::create(float duration, EffectDirection direction)
{
    ldWipeTo* pRet = new ldWipeTo();
    if (!pRet->initWithDuration(duration, direction)) return NULL;
    return pRet;
}

bool ldWipeTo::initWithDuration(float duration, EffectDirection direction)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_wipeShader.setDirection(direction);
        
        return true;
    }
    return false;
}

void ldWipeTo::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    
    pTarget->setPixelShader(&m_wipeShader);
    
    m_wipeShader.setTarget(pTarget);
    
    this->update(0);
}

void ldWipeTo::stop()
{
    m_pTarget->setPixelShader(NULL);
    
    m_wipeShader.setTarget(NULL);
    
    ldEffect::stop();
}

void ldWipeTo::update(float time)
{
    m_wipeShader.setProgress(time);
}

//
// PickTo
//
ldPickeTo* ldPickeTo::create(float duration, EffectDirection direction)
{
    ldPickeTo* pRet = new ldPickeTo();
    if (!pRet->initWithDuration(duration, direction)) return NULL;
    return pRet;
}

bool ldPickeTo::initWithDuration(float duration, EffectDirection direction)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_nDirection = direction;
        
        EffectDirection newDirection = ToTop;
        if (direction == ToTop) newDirection = ToBottom;
        if (direction == ToBottom) newDirection = ToTop;
        if (direction == ToRight) newDirection = ToLeft;
        if (direction == ToLeft) newDirection = ToRight;

        m_wipeShader.setDirection(newDirection);
        
        return true;
    }
    return false;
}

void ldPickeTo::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);
    
    pTarget->setPixelShader(&m_wipeShader);
    
    m_wipeShader.setTarget(pTarget);
    
    m_fPosition = pTarget->getPosition();
    m_fSize = pTarget->getContentSize();
    
    this->update(0);
}

void ldPickeTo::stop()
{
    m_pTarget->setPixelShader(NULL);
    
    m_wipeShader.setTarget(NULL);
    
    ldEffect::stop();
}

void ldPickeTo::update(float time)
{
    if (m_nDirection == ToTop)
    {
        m_pTarget->setPosition(ccpAdd(m_fPosition, ccp(0, (time - 1) * m_fSize.height)));
    }
    else if (m_nDirection == ToBottom)
    {
        m_pTarget->setPosition(ccpAdd(m_fPosition, ccp(0, (1 - time) * m_fSize.height)));
    }
    else if (m_nDirection == ToRight)
    {
        m_pTarget->setPosition(ccpAdd(m_fPosition, ccp((time - 1) * m_fSize.width, 0)));
    }
    else if (m_nDirection == ToLeft)
    {
        m_pTarget->setPosition(ccpAdd(m_fPosition, ccp((1 - time) * m_fSize.width, 0)));
    }
    
    m_wipeShader.setProgress(time);
}


//
// Waveform
//
ldWaveform* ldWaveform::create(float duration, float amplitude, float wavelength)
{
    ldWaveform* pRet = new ldWaveform();
    pRet->initWithDuration(duration, amplitude, wavelength);
    return pRet;
}

bool ldWaveform::initWithDuration(float duration, float amplitude, float wavelength)
{
    if (ldEffectInterval::initWithDuration(duration))
    {
        m_waveShader.setAmplitude(amplitude);
        m_waveShader.setWavelength(wavelength);
        
        return true;
    }
    
    return false;
}

void ldWaveform::startWithTarget(ldShape *pTarget)
{
    ldEffectInterval::startWithTarget(pTarget);

    pTarget->setPixelShader(&m_waveShader);
    
    m_waveShader.setTarget(pTarget);
    
    this->update(0);
}

void ldWaveform::stop()
{
    m_pTarget->setPixelShader(NULL);
    
    m_waveShader.setTarget(NULL);
    
    ldEffect::stop();
}

void ldWaveform::update(float time)
{
    m_waveShader.setStartCycle(-time * m_fDuration / 2);
}
