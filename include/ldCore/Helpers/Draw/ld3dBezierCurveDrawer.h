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
//  ld3dBezierCurveDrawer.h
//
//  Created by Sergey Gavrushkin on 18/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#ifndef LD3DBEZIERCURVEDRAWER_H
#define LD3DBEZIERCURVEDRAWER_H

#include <memory>

#include <QtCore/QTime>
#include <ldCore/Helpers/BezierCurve/ldBezierCurveFrame.h>

#include "ldCore/Helpers/Maths/ldMaths.h"

class ldAbstractColorEffect;
class ldRendererOpenlase;
class ld3dBezierCurveObject;
class ldBezierCurveObject;

// Steps3dTState
struct LDCORESHARED_EXPORT Steps3dTState {
    point3d position;
    float angleX = 0.f;
    float angleY = 0.f;
    float angleZ = 0.f;

    explicit Steps3dTState() {}
};

/**
 * Allows to draw any curve object with 3d effect appear/disappear
 */
class LDCORESHARED_EXPORT ld3dBezierCurveDrawer
{
public:
    explicit ld3dBezierCurveDrawer();
    ~ld3dBezierCurveDrawer();

    /**
     * Set bezierCurve-based objects to draw
     * @param objects
     */
    void setFrame(const ld3dBezierCurveFrame &frame);

    /**
     * Perform 3D drawing of objects from setObjectsToDraw
     * @param renderer
     * @return true if drawing was performed well. false if 3d cycle was finished and you can set new data
     */
    bool innerDraw(ldRendererOpenlase *renderer);
    // for mode rotate..
    bool innerDrawTwo(ldRendererOpenlase *renderer, int rotate_step);


    /**
     * @brief reset current state to inital
     */
    void reset();

    /**
     * @brief setSpeed - set animation speed
     * @param speed - coeff, applied to original speed
     */
    void setSpeedCoeff(float speed);

private:
    enum class Step {
        INTRO,
        SHOW,
        REMOVE
    };


    Steps3dTState getStateByIndice(uint indice, float inputCoeff) const;
    void nextStep();
    bool checkCurrentStep();

    Step _currentStep = Step::INTRO;

    int _colorStep = 0;
    int _colorNextStep = 170;

    QTime _elapsedTimer;

    ld3dBezierCurveFrame _bezier3dSequence;

    int _randomDelta = 0;

    float m_speedCoeff = 1.f;
};

#endif // LD3DBEZIERCURVEDRAWER_H

