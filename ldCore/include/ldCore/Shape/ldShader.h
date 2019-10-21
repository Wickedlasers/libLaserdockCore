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

#ifndef LaserdockEngine__ldShader__
#define LaserdockEngine__ldShader__

#include <QtCore/QStack>

#include "ldCore/Filter/ldFilter.h"
#include "ldCore/Shape/ldParticleGeometry.h"

class ldShape;


/**
 * Shader Manager
 */
class LDCORESHARED_EXPORT ldShaderManager
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
    QStack<ldShader*> m_VtxPreStack;
    QStack<ldShader*> m_VtxStack;
    QStack<ldShader*> m_PxStack;
};


#endif /* defined(__LaserdockEngine__ldShader__) */
