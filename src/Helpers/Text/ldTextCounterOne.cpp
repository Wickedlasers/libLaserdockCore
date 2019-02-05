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

// ldTextCounterOne.cpp
// Created by Eric Brugère on 1/feb/17.
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#include "ldCore/Helpers/Text/ldTextCounterOne.h"

#include <QtCore/QDebug>

#include <ldCore/Helpers/Color/ldColorUtil.h>
#include <ldCore/Helpers/ldEnumHelper.h>

#include "ldCore/Helpers/SVG/ldSvgReader.h"
#include "ldCore/Helpers/Text/ldTextLabel.h"


// ldTextCounterOne
ldTextCounterOne::ldTextCounterOne(QString p_logo_path, int p_currentCount)
    : QObject()
    , _textLabel(new ldTextLabel("", 1.f/9))
    , _messageTextLabel(new ldTextLabel("", 1.f/9))
    , _currentCount(p_currentCount)
    , _nextCount(p_currentCount)
{
    if (p_logo_path != "") {
        ldBezierCurveObject logoSvg2d = ldBezierCurveObject(ldSvgReader::loadSvg(p_logo_path, ldSvgReader::Type::Maximize, 0.01f),
                                                            false);
        logoSvg2d.scale(0.7f);
        _logoSvg = ld3dBezierCurveFrame(logoSvg2d.to3d());
    }
}

// ~ldTextCounterOne
ldTextCounterOne::~ldTextCounterOne()
{
}

// doUpdateWithoutLogo
bool ldTextCounterOne::doUpdateWithoutLogo(int p_newCount)
{
    if (_currentStep != Step::COUNT_STEADY) {
        return false;
    }

    _stepTimer.start();
    _currentStep = Step::COUNT_HIDING_FOR_NEXT;

    prepareForNext(p_newCount);

    return true;
}

// doUpdateWithLogo
bool ldTextCounterOne::doUpdateWithLogo(int p_newCount)
{
    if(doUpdateWithoutLogo(p_newCount)) {
        _doLogoOnce=true;
        return  true;
    } else {
        return false;
    }
}

// prepareForNext
void ldTextCounterOne::prepareForNext(int p_nextCount)
{
    _currentCount = _nextCount;
    _nextCount = p_nextCount;

    // detect how many digits should flip over
    // find first different digit if lengths the same, otherwise flip all
    _keyStartChangeWithNewOne = 0;
    QString currentCountStr = QString::number(_currentCount);
    QString nextCountStr = QString::number(_nextCount);
    if(currentCountStr.length() == nextCountStr.length()) {
        for (int i = 0; i < currentCountStr.length(); i++) {
            if (currentCountStr[i] != nextCountStr[i]) {
                _keyStartChangeWithNewOne = i;
                break;
            }
        }
    }
    _keyEndChangeWithNewOne = currentCountStr.length() - _keyStartChangeWithNewOne;

}

// showLogoEvery
void ldTextCounterOne::showLogoEvery(float p_second)
{
    _logoBypassCycleSec = p_second;

    _logoTimer.start();
}

QString ldTextCounterOne::message() const
{
    return _suffixMessage;
}

int ldTextCounterOne::count() const
{
    return _currentCount;
}

// nextStep
void ldTextCounterOne::nextStep()
{
    _stepTimer.start();

    bool isReset = false;
    switch(_currentStep) {
        case Step::COUNT_STEADY:
            // never happen
            return;
        case Step::COUNT_HIDING_FOR_NEXT:
            updateString();
            break;
        case Step::COUNT_SHOWING_NEXT:
            if (_doLogoOnce) {
                _doLogoOnce = false;
            } else {
                isReset = true;
            }
            break;
        case Step::COUNT_SHOWING_CURRENT:
            if(_logoBypassCycleSec > 0) {
                _logoTimer.start();
            }
            isReset = true;
            break;
        default:
            break;
    };

    if(isReset) {
        _currentStep = Step::COUNT_STEADY;
        emit finished();
    } else {
        // next step
        _currentStep = Step(ldEnumHelper::as_integer(_currentStep) + 1);
    }
}

// checkTimers
void ldTextCounterOne::checkTimers()
{
    // check if timers are started
    if(!_colorTimer.isValid()) {
        _colorTimer.start();
    }
    if(!_stepTimer.isValid()) {
        _stepTimer.start();
    }


    // check step timer
    qint64 elapsedMs = _stepTimer.elapsed();
    float elapsedStepSec = 1.0f * (elapsedMs) / 1000.0f;

    //
    float duration = 1;

    if (_currentStep == Step::COUNT_STEADY) {
        if (elapsedStepSec > duration) _stepTimer.start();
    } else if (_currentStep == Step::COUNT_HIDING_FOR_NEXT
              || _currentStep == Step::COUNT_SHOWING_NEXT) {
        duration = TIME_CHANGE_COUNT_SEC/2.0f;
        if (elapsedStepSec > duration) {
            nextStep();
        }
    } else if (_currentStep == Step::LOGO_HIDING
              || _currentStep == Step::LOGO_SHOWING
               || _currentStep == Step::COUNT_HIDING_FOR_LOGO
               || _currentStep == Step::COUNT_SHOWING_CURRENT) {
        duration = TIME_CHANGE_LOGO_SEC*TIME_TCOUNTONE_LOGO_ANIM_INTERMEDIATES_COEF;
        if (elapsedStepSec > duration) {
            nextStep();
        }
    } else if (_currentStep == Step::LOGO_STEADY) {
         duration = TIME_CHANGE_LOGO_SEC*(1-4*TIME_TCOUNTONE_LOGO_ANIM_INTERMEDIATES_COEF);
         if (elapsedStepSec > duration) {
             nextStep();
         }
     }

    if (_currentStep < Step::COUNT_HIDING_FOR_LOGO && _logoBypassCycleSec > 0) {
        qint64 elapsedLogoMs = _logoTimer.elapsed();
        float elpasedLogoSec  = 1.0f * (elapsedLogoMs) / 1000.0f;

        if (elpasedLogoSec > _logoBypassCycleSec) {
            _currentStep=Step::COUNT_HIDING_FOR_LOGO;
        }
    }

    // check color timer
    float sec_since_last_colorchange = 1.0f * (_colorTimer.elapsed()) / 1000.0f;
    if (sec_since_last_colorchange > TIME_COLOR_CYCLE_SEC) {
        _colorTimer.start();
        _colorStep = _colorNextStep;
        _colorNextStep = ldMaths::periodIntervalKeeperInt(_colorStep+170, 0, 360);
        _messageTextLabel->setColor(ldColorUtil::colorHSV(_colorStep, 1.0, 1.0));
    }
}

// innerDraw
void ldTextCounterOne::innerDraw(ldRendererOpenlase *p_renderer)
{
    checkTimers();

    float coefInput =  1.0f *_stepTimer.elapsed()/1000.f;

    float colorElapsedSec = 1.0f * _colorTimer.elapsed() / 1000.0f;
    float coefBase = colorElapsedSec/TIME_COLOR_CYCLE_SEC;
    //
    if (_currentStep == Step::COUNT_STEADY) {
        coefInput = 0;
    } else if (_currentStep == Step::COUNT_HIDING_FOR_NEXT) {

        coefInput = (coefInput)/(0.5f*TIME_CHANGE_COUNT_SEC);
    } else if (_currentStep == Step::COUNT_SHOWING_NEXT) {

        coefInput = (coefInput)/(0.5f*TIME_CHANGE_COUNT_SEC);
    } else if (_currentStep == Step::LOGO_HIDING
               || _currentStep == Step::LOGO_SHOWING
                || _currentStep == Step::COUNT_HIDING_FOR_LOGO
                || _currentStep == Step::COUNT_SHOWING_CURRENT) {
        coefInput = (coefInput)/(TIME_TCOUNTONE_LOGO_ANIM_INTERMEDIATES_COEF*TIME_CHANGE_LOGO_SEC);
    } else if (_currentStep == Step::LOGO_STEADY) {

        coefInput = (coefInput)/((1-4*TIME_TCOUNTONE_LOGO_ANIM_INTERMEDIATES_COEF)*TIME_CHANGE_LOGO_SEC);
    }


    p_renderer->loadIdentity3();
    p_renderer->pushMatrix3();

    // scale
    float s=2.20f;
    p_renderer->scale3(s, s, s);

    // camera is frustum
    // float left, float right, float bot, float top, float near, float far
    p_renderer->frustum( 1, -1, 1, -1, 1, 10);

    // scene is in front of camera
    p_renderer->translate3(0, 0, 2);

    // transform for cube
    p_renderer->pushMatrix3();

    //

    bool isLogo = false;
    bool isLogoAnim = false;


    if (_currentStep >= Step::LOGO_SHOWING && _currentStep <= Step::LOGO_HIDING) {
        isLogo = true;
    }
    if (_currentStep >= Step::COUNT_HIDING_FOR_LOGO && _currentStep <= Step::COUNT_SHOWING_CURRENT) {
        isLogoAnim = true;
    }

    const ld3dBezierCurveFrame &drawFrame = isLogo
                                             ? _logoSvg
                                             : _label3dLetters;

    int nbPoints = drawFrame.countPoints();
    nbPoints = nbPoints*nbPoints*nbPoints;

    if (nbPoints<1) nbPoints = 1;
    //
    int bezierCurveObjectIndex=0;
    for (const ld3dBezierCurveObject &bezier3dCurves : drawFrame.data()) {
        //
        Steps3dTState state = get3dState(bezierCurveObjectIndex, isLogoAnim, coefInput);

        for (const std::vector<Bezier3dCurve> &bezier3dTab : bezier3dCurves.data())
        {
            p_renderer->begin(OL_LINESTRIP);

            for (const Bezier3dCurve &b : bezier3dTab)
            {
                //
                int maxPoints = (int) (300*300*300*ldMaths::bezier3dLength(b)/nbPoints);
                if (maxPoints<2) maxPoints = 2;
                if (maxPoints>10) maxPoints = 10;

                //
                point3d piv = b.pivot;
                if (bezier3dCurves.isUnitedCoordinates()) {
                    piv = piv.toLaserCoord();
                }
                //
                for (int j=0; j<maxPoints; j++)
                {
                    float slope = 1.0f*(j)/(maxPoints-1);
                    point3d p = b.getPoint(slope);
                    float base_y = p.y - drawFrame.dim().bottom();

                    // unitedToLaserCoords
                    if (bezier3dCurves.isUnitedCoordinates()) {
                        p = p.toLaserCoord();
                    }

                    // flip
                    p.rotate(state.angleX, state.angleY, state.angleZ, piv);

                    // color
                    int colorStep = (coefBase > base_y/drawFrame.dim().height()) ? _colorNextStep : _colorStep;
                    uint32_t color = ldColorUtil::colorHSV(colorStep, 1.0, 1.0);

                    p_renderer->vertex3(p.x, p.y, p.z, color);

                }
            }
            p_renderer->end();
        }
        bezierCurveObjectIndex++;
    }

    _messageTextLabel->innerDraw(p_renderer);

    p_renderer->popMatrix3();
    p_renderer->popMatrix3();
}

void ldTextCounterOne::reset()
{

}

void ldTextCounterOne::setCurrentCount(int p_currentCount)
{
    _currentCount = _nextCount = p_currentCount;
    updateString();
}

void ldTextCounterOne::setMessage(const QString &message)
{
    _suffixMessage = message;
    _messageTextLabel->setText(_suffixMessage);
    _messageTextLabel->setPosition(Vec2((1.0f - _messageTextLabel->getWidth()) / 2.f, 0.4f));
//    updateString();
}

Steps3dTState ldTextCounterOne::get3dState(int bezierCurveObjectIndex, bool forceFlip, float coefInput)
{
    Steps3dTState state;
    // flip algorithm
    if ((bezierCurveObjectIndex >= _keyStartChangeWithNewOne && bezierCurveObjectIndex < _keyEndChangeWithNewOne)
            || forceFlip) {
        if (_currentStep == Step::COUNT_HIDING_FOR_NEXT
               || _currentStep == Step::COUNT_SHOWING_NEXT
                || _currentStep == Step::COUNT_HIDING_FOR_LOGO
                || _currentStep == Step::LOGO_SHOWING
                || _currentStep == Step::LOGO_HIDING
                || _currentStep == Step::COUNT_SHOWING_CURRENT
                ) {
            state.angleX = -0.5f*(float)M_PI;
        }


        if (_currentStep == Step::COUNT_SHOWING_NEXT
             || _currentStep == Step::LOGO_SHOWING
             || _currentStep == Step::COUNT_SHOWING_CURRENT) {
             state.angleX =  (state.angleX * coefInput - (float) M_PI/2.f - (float) M_PI);
         } else {
             state.angleX = state.angleX * coefInput;
         }
         state.angleY = state.angleY * coefInput;
         state.angleZ = state.angleZ * coefInput;
    }

    return state;
}

// updateString
void ldTextCounterOne::updateString()
{
    int countToShow = (_currentStep == Step::COUNT_HIDING_FOR_NEXT) ? _nextCount : _currentCount;

    _textLabel->setText(QString::number(countToShow));
    // get bezier frame
    ldBezierCurveFrame labelLetters = _textLabel->getFrame();

    // recenter
    if(_label3dLetters.data().empty()) {
        labelLetters.moveToCenter();
        labelLetters.translate(Vec2(0, 0.1f));
    } else {
        // align with right side
        Vec2 oldPos(_label3dLetters.dim().right(), _label3dLetters.dim().bottom());
        labelLetters.moveTo(oldPos);
        labelLetters.translate(Vec2(-1.f * labelLetters.dim().width(), 0.f));
    }

    // convert to 3d
    _label3dLetters = labelLetters.to3d();
}
