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
#include <cmath>

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
    return m_VtxPreStack.pop();
}

ldShader* ldShaderManager::popVertextShader()
{
    return m_VtxStack.pop();
}

ldShader* ldShaderManager::popPixelShader()
{
    return m_PxStack.pop();
}

void ldShaderManager::apply_vertex_pre_shader(float *x, float *y, uint32_t *color)
{
    if (m_VtxPreStack.length() == 0) return;
    
    for (auto it = m_VtxPreStack.rbegin(); it != m_VtxPreStack.rend(); ++it ) {
        ldShader* shader = *it;
        shader->ShaderFunc(x, y, color);
    }
}

void ldShaderManager::apply_vertex_shader(float *x, float *y, uint32_t *color)
{
    if (m_VtxStack.length() == 0) return;

    for (auto it = m_VtxStack.rbegin(); it != m_VtxStack.rend(); ++it ) {
        ldShader* shader = *it;
        shader->ShaderFunc(x, y, color);
    }
}

void ldShaderManager::apply_pixel_shader(float *x, float *y, uint32_t *color)
{
    if (m_PxStack.length() == 0) return;

    for (auto it = m_PxStack.rbegin(); it != m_PxStack.rend(); ++it ) {
        ldShader* shader = *it;
        shader->ShaderFunc(x, y, color);
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
