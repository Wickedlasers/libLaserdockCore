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

// ldTextCounterOne.h
// Created by Eric Brugère on 1/feb/17.
// Copyright (c) 2017 Wicked Lasers. All rights reserved.

#ifndef ldTextCounterOne_H
#define ldTextCounterOne_H

#include <string>

#include <QtCore/QElapsedTimer>
#include <QtCore/QStringList>

#include <ldCore/Render/ldRendererOpenlase.h>

#include "ldCore/Helpers/BezierCurve/ld3dBezierCurveFrame.h"
#include "ldCore/Helpers/BezierCurve/ld3dBezierCurveObject.h"
#include "ldCore/Helpers/Maths/ldMaths.h"

#include "ld3dTextLabelOne.h"

class ldTextLabel;

#define TIME_TCOUNTONE_LOGO_ANIM_INTERMEDIATES_COEF 0.20f // SHALL STAY UNDER .24 !!!

class LDCORESHARED_EXPORT ldTextCounterOne : public QObject
{
    Q_OBJECT
public:
    explicit ldTextCounterOne(QString p_logo_path = "",
                     int p_currentCount = 0);
    virtual ~ldTextCounterOne();

    void innerDraw(ldRendererOpenlase *p_renderer);

public slots:
    void reset();

    void setCurrentCount(int p_currentCount);
    void setMessage(const QString &message);

    bool doUpdateWithoutLogo(int p_newCount);
    bool doUpdateWithLogo(int p_newCount);

    void showLogoEvery(float p_second);

    QString message() const;
    int count() const;

signals:
    void finished();

private:
    enum class Step {
        COUNT_STEADY,
        COUNT_HIDING_FOR_NEXT,
        COUNT_SHOWING_NEXT,
        COUNT_HIDING_FOR_LOGO,
        LOGO_SHOWING,
        LOGO_STEADY,
        LOGO_HIDING,
        COUNT_SHOWING_CURRENT
    };


    void checkTimers();
    void prepareForNext(int p_nextCount);

    Steps3dTState get3dState(int digitIndex, bool forceFlip, float coefInput);
    void nextStep();
    void updateString();

    const float TIME_CHANGE_COUNT_SEC = 2.0f; // min 0.05f
    const float TIME_CHANGE_LOGO_SEC = 5.f; // min 0.05f
    const float TIME_COLOR_CYCLE_SEC = 2.1f; // min 0.05f

    Step _currentStep = Step::COUNT_STEADY;

    QElapsedTimer _logoTimer;
    QElapsedTimer _colorTimer;
    QElapsedTimer _stepTimer;

    float _logoBypassCycleSec = -1.f;

    int _colorStep = 0;
    int _colorNextStep = 170;

    ld3dBezierCurveFrame _logoSvg;
    ld3dBezierCurveFrame _label3dLetters;

    std::unique_ptr<ldTextLabel> _textLabel;
    std::unique_ptr<ldTextLabel> _messageTextLabel;

    int _currentCount = 0;
    int _nextCount = 0;

    int _keyStartChangeWithNewOne = 0;
    int _keyEndChangeWithNewOne = 0;
    bool _doLogoOnce = false;

    QString _suffixMessage;
};

#endif // ldTextCounterOne_H


