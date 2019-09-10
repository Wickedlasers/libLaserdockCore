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
//  ldShader.cpp
//  LaserdockEngine
//
//  Created by rock on 1/3/14.
//  Copyright (c) 2014 Wicked Lasers. All rights reserved.
//

#include <stddef.h>
#include <assert.h>
#include "ldCore/Shape/ldShader.h"
#include "ldCore/Shape/ldShape.h"

//
// Shader Manager
//

ldShaderManager* g_shaderManager = NULL;

void _vertex_pre_shader(float *x, float *y, uint32_t *color);
void _vertex_shader(float *x, float *y, uint32_t *color);
void _pixel_shader(float *x, float *y, uint32_t *color);

ldShaderManager* ldShaderManager::getSharedManager()
{
    if (g_shaderManager == NULL)
    {
        g_shaderManager = new ldShaderManager();
    }
    
    return g_shaderManager;
}

ldShaderManager::ldShaderManager()
{
}

ldShaderManager::~ldShaderManager()
{
}

void ldShaderManager::pushVertextPreShader(ldShader* shader)
{
    assert(shader);
    m_VtxPreStack.push(shader);
    
    olSetVertexPreShader(shader ? _vertex_pre_shader : NULL);
}

void ldShaderManager::pushVertextShader(ldShader* shader)
{
    assert(shader);
    m_VtxStack.push(shader);
    
    olSetVertexShader(shader ? _vertex_shader : NULL);

}

void ldShaderManager::pushPixelShader(ldShader* shader)
{
    assert(shader);
    m_PxStack.push(shader);
    
    olSetPixelShader(shader ? _pixel_shader : NULL);
}

ldShader* ldShaderManager::popVertextPreShader()
{
    return (ldShader*)m_VtxPreStack.pop();
}

ldShader* ldShaderManager::popVertextShader()
{
    return (ldShader*)m_VtxStack.pop();
}

ldShader* ldShaderManager::popPixelShader()
{
    return (ldShader*)m_PxStack.pop();
}

void ldShaderManager::apply_vertex_pre_shader(float *x, float *y, uint32_t *color)
{
    if (m_VtxPreStack.length() == 0) return;
    
    void* object;
    LDLIST_FOREACH_REVERSE((&m_VtxPreStack), object)
    {
        if (object)
        {
            ldShader* shader = (ldShader*)object;
            shader->ShaderFunc(x, y, color);
        }
    }
}

void ldShaderManager::apply_vertex_shader(float *x, float *y, uint32_t *color)
{
    if (m_VtxStack.length() == 0) return;

    void* object;
    LDLIST_FOREACH_REVERSE((&m_VtxStack), object)
    {
        if (object)
        {
            ldShader* shader = (ldShader*)object;
            shader->ShaderFunc(x, y, color);
        }
    }
}

void ldShaderManager::apply_pixel_shader(float *x, float *y, uint32_t *color)
{
    if (m_PxStack.length() == 0) return;

    void* object;
    LDLIST_FOREACH_REVERSE((&m_PxStack), object)
    {
        if (object)
        {
            ldShader* shader = (ldShader*)object;
            shader->ShaderFunc(x, y, color);
        }
    }
}


void _vertex_pre_shader(float *x, float *y, uint32_t *color)
{
    ldShaderManager::getSharedManager()->apply_vertex_pre_shader(x, y, color);
}

void _vertex_shader(float *x, float *y, uint32_t *color)
{
    ldShaderManager::getSharedManager()->apply_vertex_shader(x, y, color);
}

void _pixel_shader(float *x, float *y, uint32_t *color)
{
    ldShaderManager::getSharedManager()->apply_pixel_shader(x, y, color);
}


//
// Wipe Shader
//
ldWipeShader::ldWipeShader()
: m_nDirection(ToTop)
,m_fProgress(0.f)
,m_tPoint(CCPointZero)
,m_pTarget(NULL)
{
    
}

ldWipeShader* ldWipeShader::create()
{
    ldWipeShader* pRet = new ldWipeShader();
    
    return pRet;
}

EffectDirection ldWipeShader::getDirection()
{
    return m_nDirection;
}

void ldWipeShader::setDirection(EffectDirection direction)
{
    m_nDirection = direction;
}

//const CCPoint& ldWipeShader::getPoint()
//{
//    return m_tPoint;
//}
//
//void ldWipeShader::setPoint(const CCPoint &point)
//{
//    m_tPoint = point;
//}

float ldWipeShader::getProgress()
{
    return m_fProgress;
}

void ldWipeShader::setProgress(float progress)
{
    m_fProgress = progress;
}

ldShape* ldWipeShader::getTarget()
{
    return m_pTarget;
}

void ldWipeShader::setTarget(ldShape* pTarget)
{
    m_pTarget = pTarget;
}


void ldWipeShader::ShaderFunc(float *x, float *y, uint32_t *color)
{
    // if target shape is not sets, shader don't work.
    if (m_pTarget == NULL) return;

    float wipe_col = 1;
    
    float tx = *x;
    float ty = *y;
    
    
    CCPoint pointInNode = m_pTarget->convertToNodeSpace(ccp(tx, ty));
    tx = pointInNode.x;
    ty = pointInNode.y;
    
    m_tPoint = m_pTarget->getPosition();
    m_tSize = m_pTarget->getContentSize();
    
    float tmpBig, tmpSmall;
    if (m_nDirection == ToTop)
    {
        tmpBig = m_tSize.height * m_fProgress;
        
        wipe_col = (ty < tmpBig) ? 1 : 0;
    }
    else if (m_nDirection == ToBottom)
    {
        tmpBig = m_tSize.height * (1 - m_fProgress);
        
        wipe_col = (ty > tmpBig) ? 1 : 0;
    }
    else if (m_nDirection == ToRight)
    {
        tmpBig = m_tSize.width * m_fProgress;
        
        wipe_col = (tx < tmpBig) ? 1 : 0;
    }
    else if (m_nDirection == ToLeft)
    {
        tmpBig = m_tSize.width * (1 - m_fProgress);
        
        wipe_col = (tx > tmpBig) ? 1 : 0;
    }

    else if (m_nDirection == FromMiddleToTopAndBottom)
    {
        tmpBig = m_tPoint.y + m_tSize.height * (1 - m_fProgress / 2);
        tmpSmall = m_tPoint.y + m_tSize.height * m_fProgress / 2;
        
        wipe_col = (ty > tmpBig || ty < tmpSmall) ? 1 : 0;
    }
    else if (m_nDirection == FromMiddleToLeftAndRight)
    {
        tmpBig = m_tPoint.x + m_tSize.width * (1 - m_fProgress / 2);
        tmpSmall = m_tPoint.x + m_tSize.width * m_fProgress / 2;
        
        wipe_col = (tx > tmpBig || tx < tmpSmall) ? 1 : 0;
    }
    else if (m_nDirection == FromTopAndBottomToMiddle)
    {
        tmpBig = m_tPoint.y + m_tSize.height * (0.5 + m_fProgress / 2);
        tmpSmall = m_tPoint.y + m_tSize.height * (0.5 - m_fProgress / 2);
        
        wipe_col = (ty <= tmpBig && ty >= tmpSmall) ? 1 : 0;
    }
    else if (m_nDirection == FromLeftAndRightToMiddle)
    {
        tmpBig = m_tPoint.x + m_tSize.width * (0.5 + m_fProgress / 2);
        tmpSmall = m_tPoint.x + m_tSize.width * (0.5 - m_fProgress / 2);
        
        wipe_col = (tx <= tmpBig && tx >= tmpSmall) ? 1 : 0;
    }
    
    else if (m_nDirection == OutBox)
    {
        tmpBig = m_tPoint.y + m_tSize.height * (1 - m_fProgress / 2);
        tmpSmall = m_tPoint.y + m_tSize.height * m_fProgress / 2;
        
        wipe_col = (ty > tmpBig || ty < tmpSmall) ? 1 : 0;

        tmpBig = m_tPoint.x + m_tSize.width * (1 - m_fProgress / 2);
        tmpSmall = m_tPoint.x + m_tSize.width * m_fProgress / 2;
        
        wipe_col = (tx > tmpBig || tx < tmpSmall) ? wipe_col : 0;
    }
    else if (m_nDirection == InBox)
    {
        tmpBig = m_tPoint.y + m_tSize.height * (0.5 + m_fProgress / 2);
        tmpSmall = m_tPoint.y + m_tSize.height * (0.5 - m_fProgress / 2);
        
        wipe_col = (ty <= tmpBig && ty >= tmpSmall) ? 1 : 0;
        
        tmpBig = m_tPoint.x + m_tSize.width * (0.5 + m_fProgress / 2);
        tmpSmall = m_tPoint.x + m_tSize.width * (0.5 - m_fProgress / 2);
        
        wipe_col = (tx <= tmpBig && tx >= tmpSmall) ? wipe_col : 0;
    }
    
    
	uint8_t r,g,b;
	r = (uint8_t)(*color >> 16);
	g = (uint8_t)((*color&0xff00) >> 8);
	b = (uint8_t)(*color&0xff);
    
	r *= wipe_col;
	g *= wipe_col;
	b *= wipe_col;
    
	*color = (r<<16) | (g<<8) | b;
}


//
// ldWaveShader
//
ldWaveShader::ldWaveShader()
: m_fStartCycle(0)
, m_fAmplitude(1)
, m_fWaveLength(1)
,m_pTarget(NULL)
{
    
}

ldWaveShader* ldWaveShader::create(float amplitude, float wavelength)
{
    ldWaveShader* pRet = new ldWaveShader();
    pRet->init(amplitude, wavelength);
    return pRet;
}

bool ldWaveShader::init(float amplitude, float wavelength)
{
    m_fAmplitude = amplitude;
    m_fWaveLength = wavelength;
    
    return true;
}

float ldWaveShader::getAmplitude()
{
    return m_fAmplitude;
}

void ldWaveShader::setAmplitude(float amplitude)
{
    m_fAmplitude = amplitude;
}

float ldWaveShader::getWavelength()
{
    return m_fWaveLength;
}

void ldWaveShader::setWavelength(float wavelength)
{
    m_fWaveLength = wavelength;
}

float ldWaveShader::getStartCycle()
{
    return m_fStartCycle;
}

void ldWaveShader::setStartCycle(float startCycle)
{
    m_fStartCycle = startCycle;
}

ldShape* ldWaveShader::getTarget()
{
    return m_pTarget;
}

void ldWaveShader::setTarget(ldShape *pTarget)
{
    m_pTarget = pTarget;
}

void ldWaveShader::ShaderFunc(float *x, float *y, uint32_t * /*color*/)
{
    *y += m_fAmplitude * 0.5f * sinf((m_fStartCycle + *x) * 3.14 * m_fWaveLength);
}
