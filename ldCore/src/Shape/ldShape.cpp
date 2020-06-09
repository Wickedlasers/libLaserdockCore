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

#include "ldCore/Shape/ldShape.h"

#include <cmath>

#include "ldCore/Shape/ldShaderManager.h"

// XXX: Yes, shapes might have a sort problem once every 15 days if the app runs at 60 FPS and each frame sprites are reordered.
static ldRendererOpenlase* s_globalRender = NULL;

/*!
 * \class ldShape
 * \brief is a basic class of Laserdock vector shape system.
 * \inmodule Laserdock Project
 * \brief The ldShape class
 */

/*!
 * \brief ldShape
 */
ldShape::ldShape(void)
: m_pVertexPreShader(NULL)
, m_pVertextShader(NULL)
, m_pPixelShader(NULL)
, m_fRotationX(0.0f)
, m_fRotationY(0.0f)
, m_fScaleX(1.0f)
, m_fScaleY(1.0f)
, m_obContentSize(CCSizeMake(1, 1))
{
    setAnchorPoint(ccp(0.5f, 0.5f));

    m_pShaderManager = ldShaderManager::getSharedManager();
    m_renderer = s_globalRender;

    updateMatrix();
    Q_ASSERT_X(m_renderer != NULL,"ldShape", "You must set a global renderer of ldShape class before build of ldShape");
}

/*!
 * \brief ~ldShape
 */
ldShape::~ldShape()
{
}

void ldShape::setGlobalRenderer(ldRendererOpenlase *renderer)
{
    s_globalRender = renderer;
}

void ldShape::setRenderer(ldRendererOpenlase *renderer)
{
    Q_ASSERT_X(renderer != NULL,"ldShape", "You can't set NULL to a renderer of ldShape.");

    m_renderer = renderer;
}

ldRendererOpenlase* ldShape::getRenderer()
{
    return m_renderer;
}

/*!
 * /brief Returns the rotation of the node in degrees.
 *
 * \sa setRotation(float)
 */
float ldShape::getRotation()
{
    return m_fRotationX;
}

/*!
 * \brief Sets the rotation (angle) of the node in degrees.
 *
 * 0 is the default rotation angle.
 * Positive values rotate node clockwise, and negative values for anti-clockwise.
 *
 * The param \a fRotation is the roration of the node in degrees.
 */
void ldShape::setRotation(float newRotation)
{
    m_fRotationX = m_fRotationY = newRotation;
    updateMatrix();
}
/*!
 * \brief Gets the X rotation (angle) of the node in degrees which performs a horizontal rotation skew.
 *
 * \sa setRotationX(float)
 */
float ldShape::getRotationX()
{
    return m_fRotationX;
}
/*!
 * \brief Sets the X rotation (angle) of the node in degrees which performs a horizontal rotational skew.
 *
 * 0 is the default rotation angle.
 * Positive values rotate node clockwise, and negative values for anti-clockwise.
 *
 * The param fRotationX is the X rotation in degrees which performs a horizontal rotational skew.
 */
void ldShape::setRotationX(float fRotationX)
{
    m_fRotationX = fRotationX;
    updateMatrix();
}
/*!
 * \brief Gets the Y rotation (angle) of the node in degrees which performs a vertical rotational skew.
 *
 * \sa setRotationY(float)
 */
float ldShape::getRotationY()
{
    return m_fRotationY;
}
/*!
 * \brief Sets the Y rotation (angle) of the node in degrees which performs a vertical rotational skew.
 *
 * 0 is the default rotation angle.
 * Positive values rotate node clockwise, and negative values for anti-clockwise.
 *
 * The param \a fRotationY    The Y rotation in degrees.
 */
void ldShape::setRotationY(float fRotationY)
{
    m_fRotationY = fRotationY;
    updateMatrix();
}

/*!
 * \brief Gets the scale factor of the node,  when X and Y have the same scale factor.
 *
 * \sa setScale(float)
 */
float ldShape::getScale(void)
{
    return m_fScaleX;
}

/*!
 * \brief Changes both X and Y scale factor of the node.
 *
 * 1.0 is the default scale factor. It modifies the X and Y scale at the same time.
 *
 * The param \a scale is the scale factor for both X and Y axis.
 */
void ldShape::setScale(float scale)
{
    m_fScaleX = m_fScaleY = scale;
    updateMatrix();
}

/*!
 * \brief Returns the scale factor on X axis of this node
 *
 * \sa setScaleX(float)
 */
float ldShape::getScaleX()
{
    return m_fScaleX;
}


/*!
 * \brief Changes the scale factor on X axis of this node
 *
 * The deafult value is 1.0 if you haven't changed it before
 *
 * The param \a fScaleX is the scale factor on X axis.
 */
void ldShape::setScaleX(float newScaleX)
{
    m_fScaleX = newScaleX;
    updateMatrix();
}

/*!
 * \brief Returns the scale factor on Y axis of this node
 *
 * \sa setScaleY(float)
 */
float ldShape::getScaleY()
{
    return m_fScaleY;
}

/*!
 * \brief Changes the scale factor on Y axis of this node
 *
 * The Default value is 1.0 if you haven't changed it before.
 *
 * The param \a fScaleY is the scale factor on Y axis.
 */
void ldShape::setScaleY(float newScaleY)
{
    m_fScaleY = newScaleY;
    updateMatrix();
}

/*!
 * \brief Gets the position (x,y) of the node in OpenGL coordinates
 *
 * \sa setPosition(const CCPoint&)
 */
const CCPoint& ldShape::getPosition()
{
    return m_obPosition;
}


/*!
 * \brief Changes the position (x,y) of the node in OpenGL coordinates
 *
 * Usually we use ccp(x,y) to compose CCPoint object.
 * The original point (0,0) is at the left-bottom corner of screen.
 * For example, this codesnip sets the node in the center of screen.
 * \code
 * CCSize size = CCDirector::sharedDirector()->getWinSize();
 * node->setPosition( ccp(size.width/2, size.height/2) )
 * \endcode
 *
 * The param \a position is the position (x,y) of the node in OpenGL coordinates.
 */
void ldShape::setPosition(const CCPoint& newPosition)
{
    m_obPosition = newPosition;
    updateMatrix();
}

/*!
 * \fn const CCPoint& ldShape::getAnchorPoint()
 * \brief Returns the anchor point in percent.
 *
 * \sa setAnchorPoint(CCPoint&)
 */
const CCPoint& ldShape::getAnchorPoint()
{
    return m_obAnchorPoint;
}

const CCPoint &ldShape::getAnchorPointInPoints()
{
    return m_obAnchorPointInPoints;
}
/*!
 * \brief Sets the anchor point in percent.
 *
 * anchorPoint is the point around which all transformations and positioning manipulations take place.
 * It's like a pin in the node where it is "attached" to its parent.
 * The anchorPoint is normalized, like a percentage. (0,0) means the bottom-left corner and (1,1) means the top-right corner.
 * But you can use values higher than (1,1) and lower than (0,0) too.
 * The default anchorPoint is (0.5,0.5), so it starts in the center of the node.
 *
 * The param \a anchorPoint is the anchor point of node.
 */
void ldShape::setAnchorPoint(const CCPoint& point)
{
    if( ! point.equals(m_obAnchorPoint))
    {
        m_obAnchorPoint = point;
        m_obAnchorPointInPoints = ccp(m_obContentSize.width * m_obAnchorPoint.x, m_obContentSize.height * m_obAnchorPoint.y );
        updateMatrix();
    }
}

/*!
 * \brief Returns the untransformed size of the node.
 *
 * \sa setContentSize(const CCSize&)
 */
const CCSize& ldShape::getContentSize()
{
    return m_obContentSize;
}
/*!
 * \brief Sets the untransformed size of the node.
 *
 * The contentSize remains the same no matter the node is scaled or rotated.
 * All nodes has a size. Layer and Scene has the same size of the screen.
 *
 * The param \a contentSize is the untransformed size of the node.
 */
void ldShape::setContentSize(const CCSize & size)
{
    if ( ! size.equals(m_obContentSize))
    {
        m_obContentSize = size;

        m_obAnchorPointInPoints = ccp(m_obContentSize.width * m_obAnchorPoint.x, m_obContentSize.height * m_obAnchorPoint.y );
        updateMatrix();
    }
}

/*!
 * \brief Stops all running effects.
 */
void ldShape::cleanup()
{
}

/*!
 * \brief draw
 */
void ldShape::draw()
{

}
/*!
 * \brief visit
 */
void ldShape::visit()
{
    m_renderer->pushMatrix();

    m_renderer->multMatrix(m_matrix);

    if (m_pVertexPreShader) m_pShaderManager->pushVertextPreShader(m_pVertexPreShader);
    if (m_pVertextShader) m_pShaderManager->pushVertextShader(m_pVertextShader);
    if (m_pPixelShader) m_pShaderManager->pushPixelShader(m_pPixelShader);

    draw();

    if (m_pVertexPreShader) m_pShaderManager->popVertextPreShader();
    if (m_pVertextShader) m_pShaderManager->popVertextShader();
    if (m_pPixelShader) m_pShaderManager->popPixelShader();

    m_renderer->popMatrix();
}


//
// Shader methods
//
/*!
 * \brief Gets current VertexPreShader.
 */
ldShader* ldShape::getVertexPreShader()
{
    return m_pVertexPreShader;
}

/*!
 * \brief Sets the \a pShader to current VertexPreShader
 */
void ldShape::setVertexPreShader(ldShader *pShader)
{
    m_pVertexPreShader = pShader;
}

/*!
 * \brief Gets current VertextShader
 */
ldShader* ldShape::getVertextShader()
{
    return m_pVertextShader;
}

/*!
 * \brief Sets the \a pShader to current VertextShader
 */
void ldShape::setVertextShader(ldShader *pShader)
{
    m_pVertextShader = pShader;
}

/*!
 * \brief Gets current PixelShader
 */
ldShader* ldShape::getPixelShader()
{
    return m_pPixelShader;
}

/*!
 * \brief Sets the \a pShader to current PixelShader
 */
void ldShape::setPixelShader(ldShader *pShader)
{
    m_pPixelShader = pShader;
}


/*!
 * \brief Performs OpenGL view-matrix transformation based on position, scale, rotation and other attributes.
 */
void ldShape::updateMatrix()
{
    float x = m_obPosition.x;
    float y = m_obPosition.y;

    float cx = 1, sx = 0, cy = 1, sy = 0;
    if (m_fRotationX || m_fRotationY)
    {
        float radiansX = -CC_DEGREES_TO_RADIANS(m_fRotationX);
        float radiansY = -CC_DEGREES_TO_RADIANS(m_fRotationY);
        cx = cosf(radiansX);
        sx = sinf(radiansX);
        cy = cosf(radiansY);
        sy = sinf(radiansY);
    }

    // optimization:
    // inline anchor point calculation if skew is not needed
    // Adjusted transform calculation for rotational skew
    if (!m_obAnchorPointInPoints.equals(CCPoint::ZERO))
    {
        x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
        y += sy * -m_obAnchorPointInPoints.x * m_fScaleX +  cx * -m_obAnchorPointInPoints.y * m_fScaleY;
    }

    m_matrix[0] = cy * m_fScaleX;
    m_matrix[1] = sy * m_fScaleX;
    m_matrix[2] = 0.0f;
    m_matrix[3] = -sx * m_fScaleY;
    m_matrix[4] = cx * m_fScaleY;
    m_matrix[5] = 0.0f;
    m_matrix[6] = x;
    m_matrix[7] = y;
    m_matrix[8] = 1.0f;
}


