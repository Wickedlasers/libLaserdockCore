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

#include <openlase/libol.h>

#include <ldCore/ldCore_global.h>
#include <ldCore/Render/ldRendererOpenlase.h>
#include "ldCore/Shape/ldParticleGeometry.h"
#include "ldCore/Shape/ldList.h"
#include "ldCore/Shape/ldShader.h"


#include "ldAffineTransform.h"
#include "ldEffect.h"
#include "ldEffectManager.h"

#ifndef M_PI
#define M_PI    3.14159265358979323846264338328
#endif
#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923132169164
#endif

enum {
    kldShapeTagInvalid = -1,
};


class LDCORESHARED_EXPORT ldShape
{

public:

    ldShape();

    virtual ~ldShape();

public:
    static void setGlobalRenderer(ldRendererOpenlase* renderer);
    static ldRendererOpenlase* getGlobalRenderer();
    void setRenderer(ldRendererOpenlase* renderer);
    ldRendererOpenlase* getRenderer();

protected:
    ldRendererOpenlase* m_renderer;

public:


    virtual void setScaleX(float fScaleX);
    virtual float getScaleX();

    virtual void setScaleY(float fScaleY);
    virtual float getScaleY();



    virtual void setScale(float scale);


    virtual float getScale();


    virtual void setPosition(const CCPoint &position);


    virtual const CCPoint& getPosition();

    void setPosition(float x, float y);


    void getPosition(float* x, float* y);


    void  setPositionX(float x);

    float getPositionX(void);

    void  setPositionY(float y);

    float getPositionY(void);

    virtual void setAnchorPoint(const CCPoint& anchorPoint);


    virtual const CCPoint& getAnchorPoint();



    virtual void setContentSize(const CCSize& contentSize);

    virtual const CCSize& getContentSize();



    virtual void setVisible(bool visible);

    virtual bool isVisible();



    virtual void setRotation(float fRotation);

    virtual float getRotation();



    virtual void setRotationX(float fRotaionX);

    virtual float getRotationX();


    virtual void setRotationY(float fRotationY);

    virtual float getRotationY();


    virtual void visit(void);


    virtual void cleanup(void);

public:

    virtual void setEffectManager(ldEffectManager* effectManager);


    virtual ldEffectManager* getEffectManager();


    ldEffect* runEffect(ldEffect* effect);


    void stopAllEffects(void);


    void stopEffect(ldEffect* effect);

    /**
     * \brief Returns the numbers of effects that are running plus the ones that are schedule to run
     *
     * Composable actions are counted as 1 action. Example:
     *    If you are running 1 Sequence of 7 effects, it will return 1.
     *    If you are running 7 Sequences of 2 effects, it will return 7.
     */
    unsigned int numberOfRunningEffects(void);

protected:

    ldEffectManager* m_pEffectManager;


public:

    //
    // Shader functions
    //

    ldShader* getVertexPreShader();


    ldShader* getVertextShader();


    ldShader* getPixelShader();


    void setVertexPreShader(ldShader* pShader);

    void setVertextShader(ldShader* pShader);

    void setPixelShader(ldShader* pShader);

protected:

    ldShader* m_pVertexPreShader;
    ldShader* m_pVertextShader;
    ldShader* m_pPixelShader;
    ldShaderManager* m_pShaderManager;


public:

    //
    // Affine Transform funcs
    //


    CCPoint convertToNodeSpaceAR(const CCPoint& worldPoint);


    CCPoint convertToWorldSpaceAR(const CCPoint& nodePoint);


    CCPoint convertToNodeSpace(const CCPoint& worldPoint);


    CCPoint convertToWorldSpace(const CCPoint& nodePoint);

    /*!
     * \fn ldAffineTransform ldShape::nodeToWorldTransform()
     * \brief Returns the world affine transform matrix. The matrix is in Pixels.
     */
    virtual ldAffineTransform nodeToWorldTransform(void);


    virtual ldAffineTransform worldToNodeTransform(void);


    virtual ldAffineTransform nodeToParentTransform(void);

//    This is not needed now.
//    /**
//     * Returns the matrix that transform parent's space coordinates to the node's (local) space coordinates.
//     * The matrix is in Pixels.
//     */
//    virtual ldAffineTransform parentToNodeTransform(void);


protected:


    virtual void draw();


    void transform(void);

protected:
    float m_fRotationX;                 ///< rotation angle on x-axis
    float m_fRotationY;                 ///< rotation angle on y-axis

    float m_fScaleX;                    ///< scaling factor on x-axis
    float m_fScaleY;                    ///< scaling factor on y-axis

    float m_fVertexZ;                   ///< OpenGL real Z vertex

    CCPoint m_obPosition;               ///< position of the node

    float m_fSkewX;                     ///< skew angle on x-axis
    float m_fSkewY;                     ///< skew angle on y-axis

    CCPoint m_obAnchorPointInPoints;    ///< anchor point in points
    CCPoint m_obAnchorPoint;            ///< anchor point normalized (NOT in points)

    CCSize m_obContentSize;             ///< untransformed size of the node


//    CCAffineTransform m_sAdditionalTransform; ///< transform
//    CCAffineTransform m_sTransform;     ///< transform
//    CCAffineTransform m_sInverse;       ///< transform

//    CCCamera *m_pCamera;                ///< a camera

//    CCGridBase *m_pGrid;                ///< a grid

//    int m_nZOrder;                      ///< z-order value that affects the draw order

    ldList *m_pChildren;               ///< array of children nodes
    ldShape *m_pParent;                  ///< weak reference to parent node

    int m_nTag;                         ///< a tag. Can be any number you assigned just to identify this node

    void *m_pUserData;                  ///< A user assingned void pointer, Can be point to any cpp object
//    CCObject *m_pUserObject;            ///< A user assigned CCObject

//    CCGLProgram *m_pShaderProgram;      ///< OpenGL shader

//    ccGLServerState m_eGLServerState;   ///< OpenGL servier side state

    unsigned int m_uOrderOfArrival;     ///< used to preserve sequence while sorting children with the same zOrder

//    CCScheduler *m_pScheduler;          ///< scheduler used to schedule timers and updates

//    CCActionManager *m_pActionManager;  ///< a pointer to ActionManager singleton, which is used to handle all the actions

    bool m_bRunning;                    ///< is running

//    bool m_bTransformDirty;             ///< transform dirty flag
//    bool m_bInverseDirty;               ///< transform dirty flag
//    bool m_bAdditionalTransformDirty;   ///< The flag to check whether the additional transform is dirty
    bool m_bVisible;                    ///< is this node visible

//    bool m_bIgnoreAnchorPointForPosition; ///< true if the Anchor Point will be (0,0) when you position the ldShape, false otherwise.
    ///< Used by CCLayer and CCScene.

//    bool m_bReorderChildDirty;          ///< children order dirty flag

//    int m_nScriptHandler;               ///< script handler for onEnter() & onExit(), used in Javascript binding and Lua binding.
//    int m_nUpdateScriptHandler;         ///< script handler for update() callback per frame, which is invoked from lua & javascript.
//    ccScriptType m_eScriptType;         ///< type of script binding, lua or javascript


public:


    bool isRunning();



    virtual void setColor(const ccColor3B& color);

    virtual const ccColor3B& getColor(void);


protected:
    GLubyte		_displayedOpacity;
    GLubyte     _realOpacity;
    ccColor3B	_displayedColor;
    ccColor3B   _realColor;
    bool		_cascadeColorEnabled;
    bool        _cascadeOpacityEnabled;


private:

    unsigned int getOrderOfArrival();

    void setOrderOfArrival(unsigned int uOrderOfArrival);

    /// lazy allocs
    void childrenAlloc(void);

    /// helper that reorder a child
    void insertChild(ldShape* child/*, int z*/);

    /// Removes a child, call child->onExit(), do cleanup, remove it from children array.
    void detachChild(ldShape *child, bool doCleanup);

public:
    /// @{
    /// @name Children and Parent


    virtual void addChild(ldShape * child);


    virtual void addChild(ldShape* child/*, int zOrder*/, int tag);


    ldShape * getChildByTag(int tag);

    virtual ldList* getChildren();


    unsigned int getChildrenCount(void);


    virtual ldList* getAllDescendant();


    virtual void setParent(ldShape* parent);

    virtual ldShape* getParent();


    ////// REMOVES //////


    virtual void removeFromParent();

    virtual void removeFromParentAndCleanup(bool cleanup);

    virtual void removeChild(ldShape* child);

    virtual void removeChild(ldShape* child, bool cleanup);

    virtual void removeChildByTag(int tag);

    virtual void removeChildByTag(int tag, bool cleanup);

    virtual void removeAllChildren();

    virtual void removeAllChildrenWithCleanup(bool cleanup);

    /// @} end of Children and Parent
};
#endif // LDSHAPE_H
