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

class ldRendererOpenlase;

// ---------------------- LogoPoint ---------------------

class LogoPoint {
public:
    LogoPoint(float _x, float _y);

    float x, y;
};

// ---------------------- LogoLine ---------------------

class LogoLine {
public:
    QList<LogoPoint> points;
};

// ---------------------- Logo ---------------------

class Logo : public ldVisualizer{
    Q_OBJECT
public:

    virtual const char* getInternalName() override { return __FILE__; }
    virtual const char* visualizerName() override { return "Laserdock Logo"; }
    bool init();

    bool isFinished() const { return finished; }
    virtual void onShouldStart() override;

protected:
    virtual float targetFPS() { return 60; }
    virtual void draw(void) override;

    void render(ldRendererOpenlase* m_renderer);

    QList<LogoLine> lines;
    bool finished;
    float timer;
    float timerMax;
};

// ---------------------- LogoLaserdock ---------------------

class LogoLaserdock : public Logo {
public:
    LogoLaserdock();

};


#endif // LDLOGOLASERDOCK_H


