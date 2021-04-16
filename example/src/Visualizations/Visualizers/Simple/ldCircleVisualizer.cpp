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

#include "ldCircleVisualizer.h"

#include <ldCore/Helpers/Color/ldColorUtil.h>

#include "ldCircleVisualizer.h"

void ldCircleVisualizer::circle(ldRendererOpenlase* m_renderer, int points,int repeats, float x, float y, float r, float c, float angle, float dir, bool gradient) {
    m_renderer->begin(OL_LINESTRIP);

    float tx,ty,a;
    uint32_t color;
    while(repeats--) {
        for (int i = 0; i <= points; i++) {
            a = (i*dir)/points + angle;
            tx = r*cos(a*M_PI*2);
            ty = r*sin(a*M_PI*2);

            if (gradient) {
                int hue = (360*a + 360*2); hue %= 360;
                color = ldColorUtil::colorHSV(hue, 1, c);
            } else color = C_GREY(255*c);
            m_renderer->vertex(x+tx, y+ty, color);
        }

         angle+=a;

    }
    m_renderer->end();
}

ldCircleVisualizer::ldCircleVisualizer()
    : ldVisualizer()
{
    setPosition(ccp(1, 1));
}

ldCircleVisualizer::~ldCircleVisualizer() {}


void ldCircleVisualizer::draw()
{

    ldRendererOpenlase *r = m_renderer;
    /*
    // uncomment to set up custom render params
    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = r->rate();
    params.on_speed = 9999;
    params.off_speed = 9999;
    params.start_wait = 0;
    params.start_dwell = 0;
    params.end_wait = 0;
    params.end_dwell = 0;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
    r->setRenderParams(&params);
    */

    circle(r, 150,3, 0, 0, 1.00, 1.00, 0, 1, true);
    return;
}

