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

#ifndef __ldCore__ldCircleVisualizer__
#define __ldCore__ldCircleVisualizer__

#include "ldCore/Visualizations/ldVisualizer.h"

class ldCircleVisualizer : public ldVisualizer
{
    Q_OBJECT
public:
    static void circle(ldRendererOpenlase *m_renderer, int points, float x, float y, float r, float c, float angle, int overlap, int windup, float dir = 1, bool gradient = false);

    explicit ldCircleVisualizer();
    virtual ~ldCircleVisualizer();

    bool init();

    // ldVisualizer
    virtual QString visualizerName() const override { return "Circle"; }

protected:
    // ldVisualizer
    virtual void draw() override;
};

#endif /*__ldCore__ldCircleVisualizer__*/
