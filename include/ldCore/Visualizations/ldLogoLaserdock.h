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

#ifndef LDLOGOLASERDOCK_H
#define LDLOGOLASERDOCK_H

#include <QtCore/QList>

#include "ldVisualizer.h"
#include <ldCore/Helpers/Maths/ldVec2.h>
#include <ldCore/Filter/ldFilterColorize.h>

// ---------------------- LogoLine ---------------------

class ldLogoLine {
public:
    QList<ldVec2> points;
};

// ---------------------- Logo ---------------------

class ldLogo : public ldVisualizer
{
    Q_OBJECT

public:
    virtual QString visualizerName() const override { return "Laserdock Logo"; }
    bool init();

    bool isFinished() const { return m_finished; }
    virtual void onShouldStart() override;

protected:
    virtual float targetFPS() const override { return 60; }
    virtual void draw() override;

    ldShimmerFilter m_filter;
    QList<ldLogoLine> lines;
    bool m_finished = false;
    float m_timer = 0.f;
    constexpr static const float TIMER_MAX = 4.25f;
};

// ---------------------- LogoLaserdock ---------------------

class ldLogoLaserdock : public ldLogo {
public:
    ldLogoLaserdock();
};


#endif // LDLOGOLASERDOCK_H


