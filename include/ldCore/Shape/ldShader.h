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
//  ldShader.h
//  LaserdockEngine
//
//  Created by rock on 1/3/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#ifndef __LaserdockEngine__ldShader__
#define __LaserdockEngine__ldShader__

#include "ldCore/Filter/ldFilter.h"
#include "ldCore/Shape/ldParticleGeometry.h"
#include "ldCore/Shape/ldList.h"

class ldShape;


/**
 * Shader Manager
 */
class ldShaderManager
{
public:
    /// singlton method
    static ldShaderManager* getSharedManager();

private:
    ldShaderManager();
    ~ldShaderManager();
    
public:
    /// push/pop into shader stack functions.
    /// see libol library for more infos.
    void pushVertextPreShader(ldShader* shader);
    void pushVertextShader(ldShader* shader);
    void pushPixelShader(ldShader* shader);
    
    ldShader* popVertextPreShader();
    ldShader* popVertextShader();
    ldShader* popPixelShader();

    void apply_vertex_pre_shader(float *x, float *y, uint32_t *color);
    void apply_vertex_shader(float *x, float *y, uint32_t *color);
    void apply_pixel_shader(float *x, float *y, uint32_t *color);

protected:
    ldList m_VtxPreStack;
    ldList m_VtxStack;
    ldList m_PxStack;
};


enum EffectDirection {
    ToTop = 0,
    ToRight,
    ToBottom,
    ToLeft,
    
    FromMiddleToTopAndBottom,
    FromMiddleToLeftAndRight,
    FromTopAndBottomToMiddle,
    FromLeftAndRightToMiddle,
    
    OutBox,
    InBox,
    
    FromBottom = 0,
    FromLeft,
    FromTop,
    FromRight
};

/**
 * Wipe Shader
 */
class ldWipeShader : public ldShader
{
public:

    ldWipeShader();
    
    static ldWipeShader* create();
    
    // direction setter
    EffectDirection getDirection();
    
    // direction getter
    void setDirection (EffectDirection direction);
    
//    // boundary point getter
//    const CCPoint& getPoint();
//    
//    // boundary point setter
//    void setPoint (const CCPoint& point);
    
    // progress getter, progress is between 0 and 1
    float getProgress();
    
    // progress setter, progress is between 0 and 1
    void setProgress(float progress);
    
    // target shape getter;
    ldShape* getTarget();
    
    // target shape setter;
    void setTarget(ldShape* pTarget);
    
    // shader function
    virtual void ShaderFunc(float *x, float *y, uint32_t *color);
    
protected:
    
    EffectDirection m_nDirection;

    float m_fProgress;
    
    CCPoint m_tPoint;
    CCSize m_tSize;
    
    ldShape* m_pTarget;
};

/**
 * Wave Shader
 */
class ldWaveShader : public ldShader
{
public:
    
    ldWaveShader();
    
    static ldWaveShader* create(float amplitude, float wavelength);
    
    bool init(float amplitude, float wavelength);
    
    // amplitude getter
    float getAmplitude();
    
    // amplitude setter
    void setAmplitude(float amplitude);
    
    // wavelength getter
    float getWavelength();
    
    // wavelength setter
    void setWavelength(float wavelength);
    
    // startcycle getter, cycle is between 0 and 1
    float getStartCycle();
    
    // startcycle setter, cycle is between 0 and 1
    void setStartCycle(float startCycle);
    
    // target shape getter;
    ldShape* getTarget();
    
    // target shape setter;
    void setTarget(ldShape* pTarget);
    
    // shader function
    virtual void ShaderFunc(float *x, float *y, uint32_t *color);
    
protected:
    
    float m_fStartCycle;
    
    float m_fAmplitude, m_fWaveLength;
    
//    CCPoint m_tPoint;
//    CCSize m_tSize;
    
    ldShape* m_pTarget;
};

#endif /* defined(__LaserdockEngine__ldShader__) */
