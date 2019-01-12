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
//  ldTextEffect.h
//  Laserdock
//
//  Created by rock on 12/27/13.
//  Copyright (c) 2013 Jake Huang. All rights reserved.
//

#ifndef __Laserdock__ldTextEffect__
#define __Laserdock__ldTextEffect__

#include <math.h>

#include "ldShape.h"
#include "ldEffectManager.h"

class LDCORESHARED_EXPORT ldEffect
{
public:
    
    ldEffect();
    
    virtual ~ldEffect();
    
    /// return true if the action has finished
    virtual bool isDone(void);
    
    /**
     * called before the action start. It will also set the target.
     * 
     * @param pTarget   target text block.
     */
    virtual void startWithTarget(ldShape* pTarget);
    
    /// called after the effect has finished. It will set the 'target' to NULL
    /// you should never call this method manually. Instead, use "target->stopEffect(pEffect);"
    virtual void stop();
    
    /**
     * called on each frame with it's delta time.
     *
     * @param dt  delta time of frame.
     */
    virtual void step(float dt);
    
    /**
     * called once per frame.
     *
     * @param time time is between 0 and 1.
     *
     * For example:
     * - 0 means that the action just started
     * - 0.5 means that the action is in the middle
     * - 1 means that the action is over
     */
    virtual void update (float time);
    
protected:

    ldShape* m_pTarget;
};


class LDCORESHARED_EXPORT ldFiniteTimeEffect : public ldEffect
{
public:
    
    ldFiniteTimeEffect()
    : m_fDuration(0)
    {}
    
    virtual ~ldFiniteTimeEffect() {};
    
    /// get duration in seconds of the action
    inline float getDuration(void) { return m_fDuration; }

    /// set duration in seconds of the action
    inline void setDuration(float duration) { m_fDuration = duration; }
    
protected:
    
    /// duration in seconds
    float m_fDuration;
};


class LDCORESHARED_EXPORT ldEffectInterval : public ldFiniteTimeEffect
{
public:
    
    /// how many seconds had elapsed since the actions started to run.
    inline float getElapsed(void) { return m_elapsed; }
    
    /// initializes the action
    bool initWithDuration(float d);
    
    /// returns true if the action has finished
    virtual bool isDone(void);
    
    virtual void step(float dt);
    virtual void startWithTarget(ldShape *pTarget);
    
protected:
    
    float m_elapsed;
    
    bool   m_bFirstTick;
};


/**
 * DelayTime
 *
 * @brief Delays the action a certain amount of seconds
 */
class LDCORESHARED_EXPORT ldDelayTime : public ldEffectInterval
{
public:
    virtual void update(float time);
    
public:
    
    /** creates the action */
    static ldDelayTime* create(float d);
};

/**
 * Sequence
 */
class LDCORESHARED_EXPORT ldSequence : public ldEffectInterval
{
public:
    virtual ~ldSequence(void);
    
    /** helper constructor to create an array of sequenceable actions */
    static ldSequence* create(ldFiniteTimeEffect *pEffect, ...);
    /** helper constructor to create an array of sequence-able actions */
    static ldSequence* createWithVariableList(ldFiniteTimeEffect *pEffect1, va_list args);
    /** creates the action */
    static ldSequence* createWithTwoActions(ldFiniteTimeEffect *pEffectOne, ldFiniteTimeEffect *pEffectTwo);

public:
    /** initializes the action */
    bool initWithTwoEffects(ldFiniteTimeEffect *pEffectOne, ldFiniteTimeEffect *pEffectTwo);
    
    virtual void startWithTarget(ldShape *pTarget);
    virtual void stop(void);
    virtual void update(float t);

protected:
    ldFiniteTimeEffect *m_pEffects[2];
    float m_split;
    int m_last;

};

/**
 * Spawn
 */
class LDCORESHARED_EXPORT ldSpawn : public ldEffectInterval
{
public:
    ~ldSpawn(void);

    /** initializes the Spawn action with the 2 actions to spawn */
    bool initWithTwoActions(ldFiniteTimeEffect *pEffect1, ldFiniteTimeEffect *pEffect2);

    virtual void startWithTarget(ldShape *pTarget);
    virtual void stop(void);
    virtual void update(float time);

public:
    
    /** helper constructor to create an array of spawned actions */
    static ldSpawn* create(ldFiniteTimeEffect *pEffect1, ...);
    
    /** helper constructor to create an array of spawned actions */
    static ldSpawn* createWithVariableList(ldFiniteTimeEffect *pEffect1, va_list args);
    
    /** creates the Spawn action */
    static ldSpawn* createWithTwoActions(ldFiniteTimeEffect *pEffect1, ldFiniteTimeEffect *pEffect2);
    
protected:
    ldFiniteTimeEffect *m_pOne;
    ldFiniteTimeEffect *m_pTwo;
};


/**
 * Repeat
 *
 * Repeats an action a number of times.
 * To repeat an action forever use the CCRepeatForever action.
 */
class LDCORESHARED_EXPORT ldRepeat : public ldEffectInterval
{
public:
    ~ldRepeat(void);
    
    /** initializes a ldRepeat action. Times is an unsigned integer between 1 and pow(2,30) */
    bool initWithEffect(ldFiniteTimeEffect *pEffect, unsigned int times);
    
    virtual void startWithTarget(ldShape *pTarget);
    virtual void stop(void);
    virtual void update(float dt);
    virtual bool isDone(void);
    
public:
    
    /** creates a ldRepeat action. Times is an unsigned integer between 1 and pow(2,30) */
    static ldRepeat* create(ldFiniteTimeEffect *pEffect, unsigned int times);
    
protected:
    unsigned int m_uTimes;
    unsigned int m_uTotal;
    float m_fNextDt;
    bool m_bActionInstant;
    
    /** Inner action */
    ldFiniteTimeEffect *m_pInnerEffect;
};

/** 
 RepeatForever
 
 Repeats an action for ever.
 To repeat the an action for a limited number of times use the Repeat action.
 This action can't be Sequenceable because it is not an IntervalAction
 */
class LDCORESHARED_EXPORT ldRepeatForever : public ldEffectInterval
{
public:
    ldRepeatForever()
    : m_pInnerEffect(NULL)
    {}
    virtual ~ldRepeatForever();
    
    /** initializes the action */
    bool initWithEffect(ldEffectInterval *pEffect);
    virtual void startWithTarget(ldShape* pTarget);
    virtual void step(float dt);
    virtual bool isDone(void);
    
public:
    
    /** creates the action */
    static ldRepeatForever* create(ldEffectInterval *pEffect);
protected:
    /** Inner action */
    ldEffectInterval *m_pInnerEffect;
};


/**
 * MoveBy, MoveTo
 */
class LDCORESHARED_EXPORT ldMoveBy : public ldEffectInterval
{
public:
    static ldMoveBy* create(float duration, const CCPoint& deltaPosition);
    
public:
    bool initWithDuration(float duration, const CCPoint& deltaPosition);
    
    virtual void startWithTarget(ldShape *pTarget);
    virtual void update(float time);
    
protected:
    CCPoint m_positionDelta;
    CCPoint m_startPosition;
    CCPoint m_previousPosition;

};

class LDCORESHARED_EXPORT ldMoveTo : public ldMoveBy
{
public:
    /** creates the action */
    static ldMoveTo* create(float duration, const CCPoint& position);
    
    virtual void startWithTarget(ldShape *pTarget);

public:
    /** initializes the action */
    bool initWithDuration(float duration, const CCPoint& position);

protected:
    CCPoint m_endPosition;
};

/**
 * ScaleTo, ScaleBy
 */

class LDCORESHARED_EXPORT ldScaleTo : public ldEffectInterval
{
public:
    
    static ldScaleTo* create(float duration, float s);
    static ldScaleTo* create(float duration, float sx, float sy);
    
public:
    
    /// initializes the effect with the same scale factor for X and Y
    bool initWithDuration(float duration, float s);
    
    /// initializes the effect with X factor and Y factor.
    bool initWithDuration(float duration, float sx, float sy);
    
    virtual void startWithTarget(ldShape *pTarget);
    virtual void update(float time);

protected:
    float m_fScaleX;
    float m_fScaleY;
    float m_fStartScaleX;
    float m_fStartScaleY;
    float m_fEndScaleX;
    float m_fEndScaleY;
    float m_fDeltaX;
    float m_fDeltaY;
};

class LDCORESHARED_EXPORT ldScaleBy : public ldScaleTo
{
public:
    /// creates the action with the same scale factor for X and Y
    static ldScaleBy* create(float duration, float s);
    
    /// creates the action with and X factor and a Y factor
    static ldScaleBy* create(float duration, float sx, float sy);
    
public:
    
    virtual void startWithTarget(ldShape *pTarget);
};


/**
 * RotateTo, RotateBy
 */

class LDCORESHARED_EXPORT ldRotateTo : public ldEffectInterval
{
public:
    
    static ldRotateTo* create(float duration, float fDeltaAngle);
    static ldRotateTo* create(float duration, float fDeltaAngleX, float fDeltaAngleY);
    
public:

    /// initializes the effect
    bool initWithDuration(float duration, float fDeltaAngle);
    bool initWithDuration(float duration, float fDeltaAngleX, float fDeltaAngleY);

    
    virtual void startWithTarget(ldShape *pTarget);
    virtual void update(float time);
    
protected:
    float m_fDstAngleX;
    float m_fStartAngleX;
    float m_fDiffAngleX;
    
    float m_fDstAngleY;
    float m_fStartAngleY;
    float m_fDiffAngleY;
};

class LDCORESHARED_EXPORT ldRotateBy : public ldEffectInterval
{
public:
    /** creates the action */
    static ldRotateBy* create(float fDuration, float fDeltaAngle);
    static ldRotateBy* create(float fDuration, float fDeltaAngleX, float fDeltaAngleY);
    
public:
    /** initializes the action */
    bool initWithDuration(float fDuration, float fDeltaAngle);
    bool initWithDuration(float fDuration, float fDeltaAngleX, float fDeltaAngleY);

    virtual void startWithTarget(ldShape *pTarget);
    virtual void update(float time);

protected:
    float m_fAngleX;
    float m_fStartAngleX;
    float m_fAngleY;
    float m_fStartAngleY;
};


/**
 * WipeTo
 */
class LDCORESHARED_EXPORT ldWipeTo : public ldEffectInterval
{
public:
    static ldWipeTo* create(float duration, EffectDirection direction);
    
    // initializes the effect
    bool initWithDuration(float duration, EffectDirection direction);

    virtual void startWithTarget(ldShape *pTarget);
    virtual void stop();
    virtual void update(float time);
    
protected:
    ldWipeShader m_wipeShader;
};


/**
 * PickTo
 */
class LDCORESHARED_EXPORT ldPickeTo : public ldEffectInterval
{
public:
    static ldPickeTo* create(float duration, EffectDirection direction);
    
    // initializes the effect
    bool initWithDuration(float duration, EffectDirection direction);
    
    virtual void startWithTarget(ldShape *pTarget);
    virtual void stop();
    virtual void update(float time);
    
protected:
    
    ldWipeShader m_wipeShader;
    
    EffectDirection m_nDirection;
    
    CCPoint m_fPosition;
    CCSize m_fSize;
};


/**
 * Waveform
 */
class LDCORESHARED_EXPORT ldWaveform : public ldEffectInterval
{
public:
    static ldWaveform* create(float duration, float amplitude, float wavelength);
    
    // initializes the effect
    bool initWithDuration(float duration, float amplitude, float wavelength);
    
    virtual void startWithTarget(ldShape *pTarget);
    virtual void stop();
    virtual void update(float time);
    
protected:
    ldWaveShader m_waveShader;
};

#endif /* defined(__Laserdock__ldTextEffect__) */
