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

#ifndef LDSHAPE_H
#define LDSHAPE_H

#include <openlase/ldLibol.h>

#include <ldCore/ldCore_global.h>
#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Shape/ldParticleGeometry.h"

class ldShader;
class ldShaderManager;

class LDCORESHARED_EXPORT ldShape
{

public:
    ldShape();
    virtual ~ldShape();

public:
    static void setGlobalRenderer(ldRendererOpenlase* renderer);
    virtual void setRenderer(ldRendererOpenlase* renderer);
    ldRendererOpenlase* getRenderer();

protected:
    ldRendererOpenlase* m_renderer = nullptr;

public:
    void setScaleX(float fScaleX);
    float getScaleX();

    void setScaleY(float fScaleY);
    float getScaleY();

    void setScale(float scale);
    float getScale();

    void setPosition(const CCPoint &position);
    const CCPoint& getPosition();

    void setAnchorPoint(const CCPoint& anchorPoint);
    const CCPoint& getAnchorPoint();
    const CCPoint& getAnchorPointInPoints();

    void setContentSize(const CCSize& contentSize);
    const CCSize& getContentSize();

    void setRotation(float fRotation);
    float getRotation();

    void setRotationX(float fRotaionX);
    float getRotationX();

    void setRotationY(float fRotationY);
    float getRotationY();

    virtual void visit(void);

    virtual void cleanup(void);

public:
    //
    // Shader functions, shouldn't be set in draw() func!
    //
    ldShader* getVertexPreShader() const;
    ldShader* getVertextShader() const;
    ldShader* getPixelShader() const;

    void setVertexPreShader(ldShader* pShader);
    void setVertextShader(ldShader* pShader);
    void setPixelShader(ldShader* pShader);

protected:
    ldShader* m_pVertexPreShader;
    ldShader* m_pVertextShader;
    ldShader* m_pPixelShader;
    ldShaderManager* m_pShaderManager;

protected:
    virtual void draw();

    float m_matrix[9];

private:
    float m_fRotationX;                 ///< rotation angle on x-axis
    float m_fRotationY;                 ///< rotation angle on y-axis

    float m_fScaleX;                    ///< scaling factor on x-axis
    float m_fScaleY;                    ///< scaling factor on y-axis

    CCPoint m_obPosition;               ///< position of the node

    CCPoint m_obAnchorPointInPoints;    ///< anchor point in points
    CCPoint m_obAnchorPoint;            ///< anchor point normalized (NOT in points)

    CCSize m_obContentSize;             ///< untransformed size of the node

    void updateMatrix();

};
#endif // LDSHAPE_H
