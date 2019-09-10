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

#include "ldCore/Shape/ldShape.h"
#include <math.h>

// XXX: Yes, shapes might have a sort problem once every 15 days if the app runs at 60 FPS and each frame sprites are reordered.
static int s_globalOrderOfArrival = 1;
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
, m_fVertexZ(0.0f)
, m_obPosition(CCPointZero)
, m_fSkewX(0.0f)
, m_fSkewY(0.0f)
, m_obAnchorPointInPoints(CCPointZero)
, m_obAnchorPoint(CCPointZero)
, m_obContentSize(CCSizeMake(1, 1))
//, m_sAdditionalTransform(CCAffineTransformMakeIdentity())
//, m_pCamera(NULL)
// children (lazy allocs)
// lazy alloc
//, m_pGrid(NULL)
//, m_nZOrder(0)
, m_pChildren(NULL)
, m_pParent(NULL)
// "whole screen" objects. like Scenes and Layers, should set m_bIgnoreAnchorPointForPosition to false
, m_nTag(kldShapeTagInvalid)
// userData is always inited as nil
, m_pUserData(NULL)
//, m_pUserObject(NULL)
//, m_pShaderProgram(NULL)
//, m_eGLServerState(ccGLServerState(0))
, m_uOrderOfArrival(0)
, m_bRunning(false)
//, m_bTransformDirty(true)
//, m_bInverseDirty(true)
//, m_bAdditionalTransformDirty(false)
//, m_bIgnoreAnchorPointForPosition(false)
//, m_bReorderChildDirty(false)
//, m_nScriptHandler(0)
//, m_nUpdateScriptHandler(0)
, m_bVisible(true)
,_realColor(ccc3(255, 255, 255))
{
    setAnchorPoint(ccp(0.5f, 0.5f));

    m_pEffectManager = ldEffectManager::getSharedManager();
    m_pShaderManager = ldShaderManager::getSharedManager();
    m_renderer = getGlobalRenderer();

    Q_ASSERT_X(m_renderer != NULL,"ldShape", "You must set a global renderer of ldShape class before build of ldShape");
}

/*!
 * \brief ~ldShape
 */
ldShape::~ldShape()
{
    if(m_pChildren && m_pChildren->length() > 0)
    {
        stopAllEffects();

        void* child;
        LDLIST_FOREACH(m_pChildren, child)
        {
            ldShape* pChild = (ldShape*) child;
            if (pChild)
            {
                pChild->m_pParent = NULL;
                delete pChild;
            }
        }

        m_pChildren->removeAll();
        delete m_pChildren;
        m_pChildren = NULL;
    }
}

void ldShape::setGlobalRenderer(ldRendererOpenlase *renderer)
{
    s_globalRender = renderer;
}

ldRendererOpenlase* ldShape::getGlobalRenderer()
{
    return s_globalRender;
}

void ldShape::setRenderer(ldRendererOpenlase *renderer)
{
    Q_ASSERT_X(renderer != NULL,"ldShape", "You can't set NULL to a renderer of ldShape.");

    m_renderer = renderer;

    // sets all child's renderer
    if ( m_pChildren && m_pChildren->length() > 0 )
    {

        ldShape* pShape;

        m_pChildren->begin();

        while (m_pChildren->isNext())
        {
            pShape = (ldShape*)m_pChildren->moveNext();

            if (pShape)
            {
                pShape->setRenderer(renderer);
            }
        }
    }
}

ldRendererOpenlase* ldShape::getRenderer()
{
    return m_renderer;
}

/*!
 * \brief Returns whether or not the node accepts event callbacks.
 *
 * Running means the node accept event callbacks like onEnter(), onExit(), update()
 */
bool ldShape::isRunning()
{
    return m_bRunning;
}

/*!
 * \brief Returns a pointer to the parent node
 *
 * \sa setParent(ldShape*)
 */
ldShape * ldShape::getParent()
{
    return m_pParent;
}
/*!
 * Sets the parent node
 *
 * The param \a parent is a pointer to the parnet node
 */
void ldShape::setParent(ldShape * var)
{
    m_pParent = var;
}

/*!
 * /brief Returns the rotation of the node in degrees.
 *
 * \sa setRotation(float)
 */
float ldShape::getRotation()
{
    //CCAssert(m_fRotationX == m_fRotationY, "ldShape#rotation. RotationX != RotationY. Don't know which one to return");
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
    //m_bTransformDirty = m_bInverseDirty = true;
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
    //m_bTransformDirty = m_bInverseDirty = true;
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
    //m_bTransformDirty = m_bInverseDirty = true;
}

/*!
 * \brief Gets the scale factor of the node,  when X and Y have the same scale factor.
 *
 * \sa setScale(float)
 */
float ldShape::getScale(void)
{
    //CCAssert( m_fScaleX == m_fScaleY, "ldShape#scale. ScaleX != ScaleY. Don't know which one to return");
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
    //m_bTransformDirty = m_bInverseDirty = true;
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
    //m_bTransformDirty = m_bInverseDirty = true;
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
    //m_bTransformDirty = m_bInverseDirty = true;
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
    //m_bTransformDirty = m_bInverseDirty = true;
}

/*!
 * \brief Gets position in a more efficient way, returns two number instead of a CCPoint object
 * The params \a x and \a y are float number.
 * \sa setPosition(float, float)
 */
void ldShape::getPosition(float* x, float* y)
{
    *x = m_obPosition.x;
    *y = m_obPosition.y;
}

/*!
 * \brief Sets position in a more efficient way.
 *
 * Passing two numbers (x,y) is much efficient than passing CCPoint object.
 * This method is binded to lua and javascript.
 * Passing a number is 10 times faster than passing a object from lua to c++
 *
 * \code
 * // sample code in lua
 * local pos  = node::getPosition()  -- returns CCPoint object from C++
 * node:setPosition(x, y)            -- pass x, y coordinate to C++
 * \endcode
 *
 * The param \a x is X coordinate for position.
 * The param \a y is Y coordinate for position
 */
void ldShape::setPosition(float x, float y)
{
    setPosition(ccp(x, y));
}
/*!
 * \brief Gets X coordinate individually for position.
 */
float ldShape::getPositionX()
{
    return m_obPosition.x;
}
/*!
 * \brief Gets Y coordinate individually for position.
 */
float ldShape::getPositionY()
{
    return  m_obPosition.y;
}
/*!
 * \brief Sets x coordinate individually for position.
 * The param \a x is a X coordinate for the position.
 */
void ldShape::setPositionX(float x)
{
    setPosition(ccp(x, m_obPosition.y));
}
/*!
 * \brief Sets y coordinate individually for position.
 * The param \a y is a Y coordinate for the position.
 */
void ldShape::setPositionY(float y)
{
    setPosition(ccp(m_obPosition.x, y));
}

/*!
 * Return an array of children
 *
 * Composing a "tree" structure is a very important feature of ldShape
 * Here's a sample code of traversing children array:
 * \code
 * ldShape* node = NULL;
 * CCARRAY_FOREACH(parent->getChildren(), node)
 * {
 *     node->setPosition(0,0);
 * }
 * \endcode
 * This sample code traverses all children nodes, and set theie position to (0,0)
 */
ldList* ldShape::getChildren()
{
    return m_pChildren;
}

/*!
 * \brief Get the amount of children.
 */
unsigned int ldShape::getChildrenCount()
{
    return m_pChildren ? m_pChildren->length() : 0;
}

/*!
 * \brief generate list of all descendant, and return it
 * caller must release memory of list object after cosume it.
 */
ldList* ldShape::getAllDescendant()
{
    ldList* pList = new ldList();

    ldList* pTemp;

    if (m_pChildren && m_pChildren->length() > 0)
    {
        pList->add(m_pChildren);

        void* object;
        LDLIST_FOREACH(m_pChildren, object)
        {
            ldShape* pChild = (ldShape*)object;

            pTemp = pChild->getAllDescendant();

            pList->add(pTemp);

            delete pTemp; pTemp = NULL;
        }
    }

    return pList;
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
        //m_bTransformDirty = m_bInverseDirty = true;
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
        //m_bTransformDirty = m_bInverseDirty = true;
    }
}
/*!
 * \brief Determines if the node is visible
 * Returns true if the node is visible, false if the node is hidden.
 *
 * \sa setVisible(bool)
 */
bool ldShape::isVisible()
{
    return m_bVisible;
}

/*!
 * \brief Sets whether the node is visible
 *
 * The default value is true, a node is default to visible
 *
 * The param \a visible is true if the node is visible, false if the node is hidden.
 */
void ldShape::setVisible(bool var)
{
    m_bVisible = var;
}

/*!
 * \brief Returns color that is currently used.
 */
const ccColor3B& ldShape::getColor(void)
{
    return _realColor;
}

/*!
 * \brief Changes the color with R,G,B bytes
 */
void ldShape::setColor(const ccColor3B& color)
{
    _realColor = color;
}

/*!
 * \brief Performs OpenGL view-matrix transformation based on position, scale, rotation and other attributes.
 */
void ldShape::transform()
{
//    x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
//    y += sy * -m_obAnchorPointInPoints.x * m_fScaleX +  cx * -m_obAnchorPointInPoints.y * m_fScaleY;

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
    if (!m_obAnchorPointInPoints.equals(CCPointZero))
    {
        x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
        y += sy * -m_obAnchorPointInPoints.x * m_fScaleX +  cx * -m_obAnchorPointInPoints.y * m_fScaleY;
    }

    float matrix[9] =
    {
        cy * m_fScaleX,     sy * m_fScaleX,     0.0f,
        -sx * m_fScaleY,    cx * m_fScaleY,     0.0f,
        x,                  y,                  1.0f
    };

    m_renderer->multMatrix(matrix);
}

/*!
 * \brief Stops all running effects.
 */
void ldShape::cleanup()
{
    // actions
    this->stopAllEffects();

    // timers
    listMakeObjectsPerformSelector(m_pChildren, cleanup, ldShape*);
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
    if (!m_bVisible) return;

    m_renderer->pushMatrix();

    transform();

    if (m_pVertexPreShader) m_pShaderManager->pushVertextPreShader(m_pVertexPreShader);
    if (m_pVertextShader) m_pShaderManager->pushVertextShader(m_pVertextShader);
    if (m_pPixelShader) m_pShaderManager->pushPixelShader(m_pPixelShader);

    //draw();
	ldShader* last = m_pPixelShader;	

    if(m_pChildren && m_pChildren->length() > 0)
    {
        // sortAllChildren();
        // todo for z-order, draw children zOrder < 0
        // self draw
        this->draw();

        // todo for z-order, draw children zOrder > 0
        void* object;
        LDLIST_FOREACH(m_pChildren, object)
        {
            ldShape* child = (ldShape*)object;
            if (child)
            {
                child->visit();
            }
        }
    }
    else
    {		
        this->draw();
    }

	ldShader* next = m_pPixelShader;
	m_pPixelShader = last;
	

    if (m_pVertexPreShader) m_pShaderManager->popVertextPreShader();
    if (m_pVertextShader) m_pShaderManager->popVertextShader();
    if (m_pPixelShader) m_pShaderManager->popPixelShader();

    m_renderer->popMatrix();

	m_pPixelShader = next;

}


//
// Effects methods
//
/*!
 * \brief Sets the ldEffectManager object that is used by all actions.
 * If you set a new ldEffectManager, then previously created actions will be removed.
 *
 * The param \a actionManager is a ldEffectManager object that is used by all actions.
 */
void ldShape::setEffectManager(ldEffectManager *effectManager)
{
    if (m_pEffectManager != effectManager)
    {
        this->stopAllEffects();
        m_pEffectManager = effectManager;
    }
}
/*!
 * \brief Gets the CCActionManager object that is used by all actions.
 * \sa setActionManager(CCActionManager*)
 */
ldEffectManager* ldShape::getEffectManager()
{
    return m_pEffectManager;
}

/*!
 * \brief Executes an effect, and returns the ection that is executed.
 *
 * This shape becomes the \a effect's target. Refer to ldEffect::getTarget()
 *
 * Returns an Effect pointer
 */
ldEffect* ldShape::runEffect(ldEffect *effect)
{
    m_pEffectManager->addEffect(effect, this, !m_bRunning);
    return effect;
}

/*!
 * \brief Stops and removes all effects from the running effect list .
 */
void ldShape::stopAllEffects()
{
    m_pEffectManager->removeAllEffectFromTarget(this);
}

/*!
 * Stops and removes an effect from the running effect list.
 *
 * The param \a effect is an action object to be removed.
 */
void ldShape::stopEffect(ldEffect *effect)
{
    m_pEffectManager->removeEffect(effect);
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


//
// Affine Transform functions
//
/*!
 * \brief Converts a \a worldPoint to node (local) space coordinates. The result is in Points.
 * treating the returned/received node point as anchor relative.
 */
CCPoint ldShape::convertToNodeSpaceAR(const CCPoint& worldPoint)
{
    CCPoint nodePoint = convertToNodeSpace(worldPoint);
    return ccpSub(nodePoint, m_obAnchorPointInPoints);
}

/*!
 * \brief Converts a local \a nodePoint to world space coordinates.The result is in Points.
 * treating the returned/received node point as anchor relative.
 */
CCPoint ldShape::convertToWorldSpaceAR(const CCPoint& nodePoint)
{
    CCPoint pt = ccpAdd(nodePoint, m_obAnchorPointInPoints);
    return convertToWorldSpace(pt);
}

/*!
 * \brief Converts a \a worldPoint to node (local) space coordinates. The result is in Points.
 */
CCPoint ldShape::convertToNodeSpace(const CCPoint& worldPoint)
{
    CCPoint ret = CCPointApplyAffineTransform(worldPoint, worldToNodeTransform());
    return ret;
}

/*!
 * \brief Converts a \a nodePoint to world space coordinates. The result is in Points.
 */
CCPoint ldShape::convertToWorldSpace(const CCPoint& nodePoint)
{
    CCPoint ret = CCPointApplyAffineTransform(nodePoint, nodeToWorldTransform());
    return ret;
}

/*!
 * \fn ldAffineTransform ldShape::nodeToWorldTransform()
 * \brief Returns the world affine transform matrix. The matrix is in Pixels.
 */
ldAffineTransform ldShape::nodeToWorldTransform()
{
    ldAffineTransform t = this->nodeToParentTransform();

    for (ldShape *p = m_pParent; p != NULL; p = p->getParent())
        t = ldAffineTransformConcat(t, p->nodeToParentTransform());

    return t;
}

/*!
 * \brief Returns the inverse world affine transform matrix. The matrix is in Pixels.
 */
ldAffineTransform ldShape::worldToNodeTransform(void)
{
    return ldAffineTransformInvert(this->nodeToWorldTransform());
}

/*!
 * \brief Returns the matrix that transform the node's (local) space coordinates into the parent's space coordinates.
 * The matrix is in Pixels.
 */
ldAffineTransform ldShape::nodeToParentTransform(void)
{
    ldAffineTransform sTransform;

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
    if (!m_obAnchorPointInPoints.equals(CCPointZero))
    {
        x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
        y += sy * -m_obAnchorPointInPoints.x * m_fScaleX +  cx * -m_obAnchorPointInPoints.y * m_fScaleY;
    }

//    float matrix[9] =
//    {
//		cy * m_fScaleX,     sy * m_fScaleX,     0.0f,
//		-sx * m_fScaleY,    cx * m_fScaleY,     0.0f,
//		x,                  y,                  1.0f
//	};

    sTransform = ldAffineTransformMake( cy * m_fScaleX, sy * m_fScaleX,
                                       -sx * m_fScaleY, cx * m_fScaleY,
                                       x, y);

//    //if (m_bTransformDirty)
//    {
//
//        // Translate values
//        float x = m_obPosition.x;
//        float y = m_obPosition.y;
//
//        //if (m_bIgnoreAnchorPointForPosition)
//        {
//            x += m_obAnchorPointInPoints.x;
//            y += m_obAnchorPointInPoints.y;
//        }
//
//        // Rotation values
//		// Change rotation code to handle X and Y
//		// If we skew with the exact same value for both x and y then we're simply just rotating
//        float cx = 1, sx = 0, cy = 1, sy = 0;
//        if (m_fRotationX || m_fRotationY)
//        {
//            float radiansX = -CC_DEGREES_TO_RADIANS(m_fRotationX);
//            float radiansY = -CC_DEGREES_TO_RADIANS(m_fRotationY);
//            cx = cosf(radiansX);
//            sx = sinf(radiansX);
//            cy = cosf(radiansY);
//            sy = sinf(radiansY);
//        }
//
//        bool needsSkewMatrix = ( m_fSkewX || m_fSkewY );
//
//
//        // optimization:
//        // inline anchor point calculation if skew is not needed
//        // Adjusted transform calculation for rotational skew
//        if (! needsSkewMatrix && !m_obAnchorPointInPoints.equals(CCPointZero))
//        {
//            x += cy * -m_obAnchorPointInPoints.x * m_fScaleX + -sx * -m_obAnchorPointInPoints.y * m_fScaleY;
//            y += sy * -m_obAnchorPointInPoints.x * m_fScaleX +  cx * -m_obAnchorPointInPoints.y * m_fScaleY;
//        }
//
//
//        // Build Transform Matrix
//        // Adjusted transform calculation for rotational skew
//        sTransform = ldAffineTransformMake( cy * m_fScaleX,  sy * m_fScaleX,
//                                             -sx * m_fScaleY, cx * m_fScaleY,
//                                             x, y );
//
//        // XXX: Try to inline skew
//        // If skew is needed, apply skew and then anchor point
//        if (needsSkewMatrix)
//        {
//            ldAffineTransform skewMatrix = ldAffineTransformMake(1.0f, tanf(CC_DEGREES_TO_RADIANS(m_fSkewY)),
//                                                                 tanf(CC_DEGREES_TO_RADIANS(m_fSkewX)), 1.0f,
//                                                                 0.0f, 0.0f );
//            sTransform = ldAffineTransformConcat(skewMatrix, sTransform);
//
//            // adjust anchor point
//            if (!m_obAnchorPointInPoints.equals(CCPointZero))
//            {
//                sTransform = ldAffineTransformTranslate(sTransform, -m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
//            }
//        }
//
////        if (m_bAdditionalTransformDirty)
////        {
////            m_sTransform = CCAffineTransformConcat(m_sTransform, m_sAdditionalTransform);
////            m_bAdditionalTransformDirty = false;
////        }
////
////        m_bTransformDirty = false;
//    }

    return sTransform;
}

unsigned int ldShape::getOrderOfArrival()
{
    return m_uOrderOfArrival;
}

void ldShape::setOrderOfArrival(unsigned int uOrderOfArrival)
{
    m_uOrderOfArrival = uOrderOfArrival;
}



// lazy allocs
void ldShape::childrenAlloc(void)
{
    m_pChildren = new ldList();
}

/*!
 * \brief Gets a child from the container with its \a aTag
 *
 * The param tag is an identifier to find the child node.
 */
ldShape* ldShape::getChildByTag(int aTag)
{
    // Invalid tag
    if (aTag == kldShapeTagInvalid) return NULL;

    if(m_pChildren && m_pChildren->length() > 0)
    {
        void* child = NULL;
        LDLIST_FOREACH(m_pChildren, child)
        {
            ldShape* pNode = (ldShape*) child;
            if(pNode && pNode->m_nTag == aTag)
                return pNode;
        }
    }
    return NULL;
}


/* "add" logic MUST only be on this method
 * If a class want's to extend the 'addChild' behavior it only needs
 * to override this method
 */
/*!
 * \brief  Adds a \a child to the container with z order and \a tag
 *
 * If the child is added to a 'running' node, then 'onEnter' and 'onEnterTransitionDidFinish' will be called immediately.
 *
 * The param child is a child node.
 * The param zOrder is a Z order for drawing priority. Please refer to setZOrder(int)
 * The param tag is a interger to identify the node easily. Please refer to setTag(int)
 */
void ldShape::addChild(ldShape *child/*, int zOrder*/, int tag)
{
    // Argument must be non-nil
    if (child == NULL) return;

    // Child already added. It can't be added again
    if (child->m_pParent != NULL) return;

    // set a renderer of child as parent's renderer.
    child->setRenderer(getRenderer());

    if( ! m_pChildren )
    {
        this->childrenAlloc();
    }

    this->insertChild(child/*, zOrder*/);

    child->m_nTag = tag;

    child->setParent(this);
    child->setOrderOfArrival(s_globalOrderOfArrival++);

//    if( m_bRunning )
//    {
//        child->onEnter();
//        child->onEnterTransitionDidFinish();
//    }
}

/*!
 * \brief Adds a child to the container with z-order as 0.
 * If the child is added to a 'running' node, then 'onEnter' and 'onEnterTransitionDidFinish' will be called immediately.
 * The param \a child is a child node
 */
void ldShape::addChild(ldShape *child)
{
    // Argument must be non-nil
    if (child == NULL) return;

    this->addChild(child/*, child->m_nZOrder*/, child->m_nTag);
}

/*!
 * \brief Removes this node itself from its parent node with a cleanup.
 * If the node orphan, then nothing happens.
 * \sa removeFromParentAndCleanup(bool)
 */
void ldShape::removeFromParent()
{
    this->removeFromParentAndCleanup(true);
}

/*!
 * \brief Removes this node itself from its parent node.
 * If the node orphan, then nothing happens.
 * The param \a cleanup is \c true if all actions and callbacks on this node should be removed, \c false otherwise.
 */
void ldShape::removeFromParentAndCleanup(bool cleanup)
{
    if (m_pParent != NULL)
    {
        m_pParent->removeChild(this,cleanup);
    }
}

/*!
 * \brief Removes a child from the container with a cleanup
 *
 * \sa removeChild(ldShape, bool)
 *
 * The param child is the child node which will be removed.
 */
void ldShape::removeChild(ldShape* child)
{
    this->removeChild(child, true);
}

/* "remove" logic MUST only be on this method
 * If a class want's to extend the 'removeChild' behavior it only needs
 * to override this method
 */
/*!
 * \brief Removes a child from the container. It will also cleanup all running actions depending on the cleanup parameter.
 *
 * The param \a child is the child node which will be removed.
 * The param \a cleanup is \c true if all running actions and callbacks on the child node will be cleanup, \c false otherwise.
 */
void ldShape::removeChild(ldShape* child, bool cleanup)
{
    // explicit nil handling
    if (m_pChildren == NULL)
    {
        return;
    }

    if ( m_pChildren->containsObject(child) )
    {
        this->detachChild(child,cleanup);
    }
}

/*!
 * \brief Removes a child from the container by \a tag value with a cleanup.
 *
 * \sa removeChildByTag(int, bool)
 *
 * The param \a tag is an interger number that identifies a child node
 */
void ldShape::removeChildByTag(int tag)
{
    this->removeChildByTag(tag, true);
}

/*!
 * \brief Removes a child from the container by tag value. It will also cleanup all running actions depending on the cleanup parameter
 *
 * The param \a tag is an interger number that identifies a child node
 * The param \a cleanup is \c true if all running actions and callbacks on the child node will be cleanup, \c false otherwise.
 */
void ldShape::removeChildByTag(int tag, bool cleanup)
{
    // Invalid tag
    if (tag == kldShapeTagInvalid) return;

    ldShape *child = this->getChildByTag(tag);

    if (child == NULL)
    {
        // removeChildByTag: child not found!
    }
    else
    {
        this->removeChild(child, cleanup);
    }
}

/*!
 * \brief Removes all children from the container with a cleanup.
 *
 * \sa removeAllChildrenWithCleanup(bool)
 */
void ldShape::removeAllChildren()
{
    this->removeAllChildrenWithCleanup(true);
}

/*!
 * \brief Removes all children from the container, and do a cleanup to all running actions depending on the cleanup parameter.
 *
 * The param \a cleanup is \c true if all running actions on all children nodes should be cleanup, \c false oterwise.
 */
void ldShape::removeAllChildrenWithCleanup(bool cleanup)
{
    // not using detachChild improves speed here
    if ( m_pChildren && m_pChildren->length() > 0 )
    {

        ldShape* pShape;

        m_pChildren->begin();

        while (m_pChildren->isNext())
        {
            pShape = (ldShape*)m_pChildren->moveNext();

            if (pShape)
            {
                // IMPORTANT:
                //  -1st do onExit
                //  -2nd cleanup
                if(m_bRunning)
                {
                    //pShape->onExitTransitionDidStart();
                    //pShape->onExit();
                }

                if (cleanup)
                {
                    pShape->cleanup();
                }
                // set parent nil at the end
                pShape->setParent(NULL);
            }
        }


        m_pChildren->removeAll();
    }

}

void ldShape::detachChild(ldShape *child, bool doCleanup)
{
    // IMPORTANT:
    //  -1st do onExit
    //  -2nd cleanup
    if (m_bRunning)
    {
        //child->onExitTransitionDidStart();
        //child->onExit();
    }

    // If you don't do cleanup, the child's actions will not get removed and the
    // its scheduledSelectors_ dict will not get released!
    if (doCleanup)
    {
        child->cleanup();
    }

    // set parent nil at the end
    child->setParent(NULL);

    m_pChildren->removeByValue(child);
}

// helper used by reorderChild & add
void ldShape::insertChild(ldShape* child/*, int z*/)
{
    m_pChildren->add(child);
}
