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
//  ld3dBezierCurveDrawer.cpp
//
//  Created by Sergey Gavrushkin on 18/02/17.
//  Copyright (c) 2017 Wicked Lasers. All rights reserved.
//

#include "ldCore/Helpers/Draw/ld3dBezierCurveDrawer.h"

#include <QtDebug>

#include <QtCore/QTime>

#include <ldCore/Helpers/ldEnumHelper.h>
#include <ldCore/Helpers/ldRandomGenerator.h>
#include <ldCore/Helpers/Color/ldColorUtil.h>
#include <ldCore/Render/ldRendererOpenlase.h>

#define TIME_FOR_INTRO_3DTO 0.8f
#define TIME_FOR_SHOW_3DTO 1.0f
#define TIME_FOR_REMOVE_3DTO 0.8f


ld3dBezierCurveDrawer::ld3dBezierCurveDrawer()
    : _currentStep(Step::INTRO)
{
}

ld3dBezierCurveDrawer::~ld3dBezierCurveDrawer()
{
}

void ld3dBezierCurveDrawer::setFrame(const ld3dBezierCurveFrame &frame)
{
    _bezier3dSequence = frame;

    // generate random delta so animation order is different for each frame
    _randomDelta = ldRandomGenerator::instance()->generate(0, 8);
}

bool ld3dBezierCurveDrawer::innerDraw(ldRendererOpenlase *renderer)
{
    if(!_elapsedTimer.isValid()) {
        _elapsedTimer.start();
    }

    if(!checkCurrentStep()) {
        return false;
    }

    float elapsedSec = 1.f * _elapsedTimer.elapsed() / 1000.f;
    float colorBaseCoeff = 0.f;
    float inputCoeff = 0.f;
    switch(_currentStep) {
    case Step::INTRO:
        inputCoeff = 1.f - elapsedSec/TIME_FOR_INTRO_3DTO*m_speedCoeff;
        break;
    case Step::SHOW:
        colorBaseCoeff = elapsedSec/TIME_FOR_SHOW_3DTO*m_speedCoeff;
        inputCoeff = 0.f;
        break;
    case Step::REMOVE:
        inputCoeff = elapsedSec/TIME_FOR_REMOVE_3DTO*m_speedCoeff;
        break;
    }

    renderer->loadIdentity3();
    renderer->pushMatrix3();

    // scale
    float s=2.20f;
    renderer->scale3(s, s, s);

    // camera is frustum
    // float left, float right, float bot, float top, float near, float far
    renderer->frustum( 1, -1, 1, -1, 1, 10);

    // scene is in front of camera
    renderer->translate3(0, 0, 2);

    // transform for cube
    renderer->pushMatrix3();

    //
    for (uint objectIndex = 0; objectIndex < _bezier3dSequence.data().size(); objectIndex++) {
        //
        const ld3dBezierCurveObject &bezier3dCurves = _bezier3dSequence.data()[objectIndex];
        Steps3dTState st = getStateByIndice(objectIndex, inputCoeff);
        //
        for (const std::vector<Bezier3dCurve> &bezier3dTab : bezier3dCurves.data())
        {
            renderer->begin(OL_LINESTRIP);
            for (const Bezier3dCurve &b : bezier3dTab) {
                // detect max points
                int maxPoints = 8;//(int) (100*b.length());
                if(maxPoints < 3) maxPoints = 3;
                if(maxPoints > 30) maxPoints = 30;

                // united to laser coord for 2d only
                point3d piv = b.pivot;
                if(bezier3dCurves.isUnitedCoordinates()) {
                    piv.toLaserCoord();
                }
                piv.z = 0;

                for (int j=0; j<maxPoints; j++)
                {
                    // get next point in curve
                    float slope = 1.0f*j/(maxPoints-1);

                    point3d p = b.getPoint(slope);

                    float base_y = p.y - _bezier3dSequence.dim().bottom();

                    // united to laser coord for 2d only
                    if(bezier3dCurves.isUnitedCoordinates()) {
                        p = p.toLaserCoord();
                    }

                    // rotation
                    p.rotate(st.angleX, st.angleY, st.angleZ, piv);

                    // 3d effect
                    float dist = st.position.distance(piv);
                    point3d t = st.position - piv;
                    t.norm();
                    t *= inputCoeff*dist;
                    p += t;

                    // detect which color step should be used depending on current state
                    int colorStep = 0;
                    switch (_currentStep) {
                    case Step::INTRO:
                        colorStep = _colorStep;
                    break;
                    case Step::SHOW:
                        colorStep = (colorBaseCoeff > base_y/ _bezier3dSequence.dim().height()) ? _colorNextStep : _colorStep;
                    break;
                    case Step::REMOVE:
                        colorStep = _colorNextStep;
                    break;
                    }
                    // convert color step to openlase format
                    uint32_t color = ldColorUtil::colorHSV(colorStep, 1.0f, 1.0f);
                    // color fade in/fade out
                    color = ldColorUtil::lerpInt(color, 0x000000, ldMaths::normLog(inputCoeff, 3.0f) );
                    // add point
                    renderer->vertex3(p.x, p.y, p.z, color);
                }
            }
            renderer->end();
        }
    }

    renderer->popMatrix3();
    renderer->popMatrix3();

    return true;
}

// innerDrawTwo
bool ld3dBezierCurveDrawer::innerDrawTwo(ldRendererOpenlase *renderer, int rotate_step)
{
    if(!_elapsedTimer.isValid()) {
        _elapsedTimer.start();
    }

    if(!checkCurrentStep()) {
        return false;
    }

    float elapsedSec = 1.f * _elapsedTimer.elapsed() / 1000.f;
    float colorBaseCoeff = 0.f;
    float inputCoeff = 0.f;
    switch(_currentStep) {
    case Step::INTRO:
        inputCoeff = 1.f - elapsedSec/TIME_FOR_INTRO_3DTO*m_speedCoeff;
        break;
    case Step::SHOW:
        colorBaseCoeff = elapsedSec/TIME_FOR_SHOW_3DTO*m_speedCoeff;
        inputCoeff = 0.f;
        break;
    case Step::REMOVE:
        inputCoeff = elapsedSec/TIME_FOR_REMOVE_3DTO*m_speedCoeff;
        break;
    }

    renderer->loadIdentity3();
    renderer->pushMatrix3();

    // scale
    float s=2.20f;
    renderer->scale3(s, s, s);

    // camera is frustum
    // float left, float right, float bot, float top, float near, float far
    renderer->frustum( 1, -1, 1, -1, 1, 10);

    // scene is in front of camera
    renderer->translate3(0, 0, 2);

    // transform for cube
    renderer->pushMatrix3();

    //
    Svg3dDim dimSequence = _bezier3dSequence.dim();
    point3d pointRot;
    pointRot.x = dimSequence.left();
    pointRot.y = dimSequence.top() - _bezier3dSequence.dim().bottom();

    point3d pivo = point3d::X_VECTOR;

    switch (rotate_step) {
    case 1:
        pivo = point3d::Y_VECTOR;
        pointRot.x = dimSequence.right() * 2.0f - 1.0f;
        break;
    case 2:
        pointRot.y = (dimSequence.bottom())*2.0f - 1.0f;
        break;
    case 3:
        pivo = point3d::Y_VECTOR;
        pointRot.y = dimSequence.bottom() - _bezier3dSequence.dim().bottom();
        pointRot.x = dimSequence.left() * 2.0f - 1.0f;
        break;
    default:
        break;
    }

    //
    for (uint objectIndex = 0; objectIndex < _bezier3dSequence.data().size(); objectIndex++) {
        //
        const ld3dBezierCurveObject &bezier3dCurves = _bezier3dSequence.data()[objectIndex];
        //
        for (const std::vector<Bezier3dCurve> &bezier3dTab : bezier3dCurves.data())
        {
            renderer->begin(OL_LINESTRIP);
            for (const Bezier3dCurve &b : bezier3dTab) {
                // detect max points
                int maxPoints = 6; //(int) (100*b.length());

                for (int j=0; j<maxPoints; j++)
                {
                    // get next point in curve
                    float slope = 1.0f*j/(maxPoints-1);

                    point3d p = b.getPoint(slope);

                    float base_y = p.y - _bezier3dSequence.dim().bottom();

                    // united to laser coord for 2d only
                    if(bezier3dCurves.isUnitedCoordinates()) {
                        p = p.toLaserCoord();
                    }

                    // rotation
                    pointRot.z = p.z;
                    p = ldMaths::rotate3dAtPoint(p, M_PI_2*inputCoeff, pivo, pointRot);

                    // detect which color step should be used depending on current state
                    int colorStep = 0;
                    switch (_currentStep) {
                    case Step::INTRO:
                        colorStep = _colorStep;
                    break;
                    case Step::SHOW:
                        colorStep = (colorBaseCoeff > base_y/ _bezier3dSequence.dim().height()) ? _colorNextStep : _colorStep;
                    break;
                    case Step::REMOVE:
                        colorStep = _colorNextStep;
                    break;
                    }
                    // convert color step to openlase format
                    uint32_t color = ldColorUtil::colorHSV(colorStep, 1.0f, 1.0f);
                    // color fade in/fade out
                    color = ldColorUtil::lerpInt(color, 0x000000, ldMaths::normLog(inputCoeff, 3.0f) );
                    // add point
                    renderer->vertex3(p.x, p.y, p.z, color);
                }
            }
            renderer->end();
        }
    }

    renderer->popMatrix3();
    renderer->popMatrix3();

    return true;
}

void ld3dBezierCurveDrawer::reset()
{
    _currentStep = Step::INTRO;
    _colorStep = 0;
    _colorNextStep = 170;
    _elapsedTimer = QTime();

    // generate random delta so animation order is different for each frame
    _randomDelta = ldRandomGenerator::instance()->generate(0, 8);
}

void ld3dBezierCurveDrawer::setSpeedCoeff(float speed)
{
    float coeffDiff = speed / m_speedCoeff;
    int elapsed = _elapsedTimer.elapsed();
    elapsed *= coeffDiff;
    _elapsedTimer = ldMaths::timeFromMs(elapsed);
//    qDebug() <<
    m_speedCoeff = speed;
}

Steps3dTState ld3dBezierCurveDrawer::getStateByIndice(uint indice, float inputCoeff) const
{
    Steps3dTState res;
    res.angleZ = (float) M_PI_2;


    int animationVariation = indice + _randomDelta;

    // 8 different variations of 3d animation
    switch (animationVariation % 8) {
        case 0:
            res.position.z = -1.40f;
            break;
        case 1:
            res.position.x = -0.3f;
            res.angleY = (float) M_PI_2;
            res.angleZ = 0;
            break;
        case 2:
            res.position.x = 0.3f;
            res.angleY = (float) M_PI_2;
            res.angleZ = 0;
            break;
        case 3:
            res.position.z = 1.90f;
            break;
        case 4:
            res.position.y = -0.3f;
            res.angleX = (float) M_PI_2;
            res.angleZ = 0;
            break;
        case 5:
            res.position.y = 0.3f;
            res.angleX = (float) M_PI_2;
            res.angleZ = 0;
            break;
        case 6:
            res.position.y = -0.3f;
            res.angleY = (float) M_PI_2;
            res.angleZ = 0;
            break;
        case 7:
            res.position.y = 0.3f;
            res.angleY = (float) M_PI_2;
            res.angleZ = 0;
            break;
        default:
            break;
    }

    res.angleX = res.angleX * inputCoeff;
    res.angleY = res.angleY * inputCoeff;
    res.angleZ = res.angleZ * inputCoeff;

    return res;
}

// nextStep
void ld3dBezierCurveDrawer::nextStep()
{
    _elapsedTimer.start();

    if(_currentStep == Step::REMOVE) {
        // reset step
        _currentStep = Step::INTRO;

        _colorStep = _colorNextStep;
        _colorNextStep = ldMaths::periodIntervalKeeperInt(_colorStep+170, 0, 360);
    } else {
        // next step
        _currentStep = Step(ldEnumHelper::as_integer(_currentStep) + 1);
    }
}

// stepChecker
bool ld3dBezierCurveDrawer::checkCurrentStep()
{
    float sec_since_last_start = 1.0f * (_elapsedTimer.elapsed()) / 1000.0f;

    float stopTime = 0.f;
    switch (_currentStep) {
    case Step::INTRO:
        stopTime = TIME_FOR_INTRO_3DTO;
        break;
    case Step::SHOW:
        stopTime = TIME_FOR_SHOW_3DTO;
        break;
    case Step::REMOVE:
        stopTime = TIME_FOR_REMOVE_3DTO;
        break;
    }

    if (sec_since_last_start > stopTime/m_speedCoeff) {
        nextStep();
        return _currentStep != Step::INTRO;
    }

    return true;
}
