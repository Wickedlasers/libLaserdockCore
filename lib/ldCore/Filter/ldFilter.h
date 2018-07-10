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

#ifndef LDFILTER
#define LDFILTER

#include "ldCore/ldCore_global.h"
#include "ldCore/Utilities/ldBasicDataStructures.h"

/**
 * Shader
 */
class LDCORESHARED_EXPORT ldShader
{
public:
    virtual void ShaderFunc(float *x, float *y, uint32_t *color) = 0;
    virtual ~ldShader(){}
};


class LDCORESHARED_EXPORT ldFilter : ldShader
{
public:
    virtual ~ldFilter(){}

    // filters implement this function to filter points.
    // input is the source vertex, processing is done in-place with a ref param
    virtual void process(Vertex &input);

    virtual QString name() { return QString(); }

    virtual bool isMusicAware() const { return false; }
    // shader function adapter
private:
    virtual void ShaderFunc(float *x, float *y, uint32_t *color);
};

#endif // LDFILTER

