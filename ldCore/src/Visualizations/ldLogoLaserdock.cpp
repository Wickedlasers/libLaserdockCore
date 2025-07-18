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

#include "ldCore/Visualizations/ldLogoLaserdock.h"

#include <ldCore/Render/ldRendererOpenlase.h>

#include "ldCore/Helpers/Maths/ldGeometryUtil.h"

// ---------------------- Logo ---------------------

void ldLogo::onShouldStart()
{
    QMutexLocker lock(&m_mutex);

    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = m_renderer->rate();
    params.on_speed = 2.0f/100.0f;
    params.off_speed = 2.0f/100.0f;
    params.start_wait = 4;
    params.start_dwell = 2;
    params.end_dwell = 1;

    params.corner_dwell = 2;
    params.curve_angle = cosf(15.0f*((float) M_PI/180.0f)); // 15 deg

    //params.max_framelen = 500;
    params.max_framelen = 30000/60 -2;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER;
    m_renderer->setRenderParams(&params);
}

void ldLogo::draw()
{
    QMutexLocker lock(&m_mutex);


    m_renderer->loadIdentity();
    m_renderer->loadIdentity3();

    m_timer += 1.0f/60.0f;
    if (m_timer >= TIMER_MAX) {
        m_timer = TIMER_MAX;
        m_finished = true;
    }

    float progress = m_timer / TIMER_MAX;

    float opacity = 1;
    if (progress > 0.68) {
        float s = (progress - 0.68) * 3;
        clampfp(s, 0, 1);
        opacity = 1-s*s;
        s = 3*(s*s*s);
        s = 3.5 / (3.5 - s);
        m_renderer->scale(s, s);
    }


    //float fstart = progress*1.5-0.5;
    //float fend = progress*1.5;
    float p2 = (progress / 0.64);
    clampfp(p2, 0, 1);
    p2 = rcostc((p2*p2)/2); // funky easement curve
    float fstart = p2*0.5;
    float fend = p2*1.0;


    clampfp(fstart, 0, 1);
    clampfp(fend, 0, 1);

    uint32_t color = C_BLUE_I(255*opacity);

    for(const ldLogoLine &line : lines) {
        //LogoPoint p;
        int points = static_cast<int>(line.points.size());
        int istart = fstart*(points-1);
        int iend = fend*(points-1);

        m_renderer->begin(OL_LINESTRIP);
        for (int i = istart; i <= iend; i++) {
            const ldVec2 &r = line.points[i];

            float x = r.x;
            float y = r.y;
            if (i == istart) {
                float f = fstart*(points-1) - istart;
                if (i+1 < points) {
                    x = (1-f)*x + (f)*line.points[i+1].x;
                    y = (1-f)*y + (f)*line.points[i+1].y;
                }
            }
            m_renderer->vertex(x, y, color);
            if (i == iend) {
                float f = fend*(points-1) - iend;
                if (i+1 < points) {
                    x = (1-f)*x + (f)*line.points[i+1].x;
                    y = (1-f)*y + (f)*line.points[i+1].y;
                }
                m_renderer->vertex(x, y, color);
            }
        }
        m_renderer->end();
    }
}

// ---------------------- LogoLaserdock ---------------------

ldLogoLaserdock::ldLogoLaserdock() {
    /*{
         LogoLine l;
         l.points.push_back(LogoPoint( 0.00,  0.00));
         l.points.push_back(LogoPoint( 0.76,  0.55));
         l.points.push_back(LogoPoint(-0.76,  0.45));
         l.points.push_back(LogoPoint(-0.46, -0.35));
         lines.append(l);
         }*/
    {
        setPixelShader(&m_filter);

        ldLogoLine l;
        for(int i = 0; i < 2; i++) {
            l.points.push_back(ldVec2(1402,532));
            l.points.push_back(ldVec2(1068,1120));
            l.points.push_back(ldVec2(1369,1120));
            l.points.push_back(ldVec2(1414,1046));
            l.points.push_back(ldVec2(1201,1046));
            l.points.push_back(ldVec2(1446,616));
        }

        l.points.push_back(ldVec2(1402,532));

        float s = 1.0f/800.0f;
        float cx = 1400;
        float cy = 930;
        for (ldVec2 &p : l.points) {
            p.x -= cx;
            p.y -= cy;
            p.x *= s;
            p.y *= -s;
        }

        lines.append(l);
    }
    {
        ldLogoLine l;
        for(int i = 0; i < 2; i++) {
            l.points.push_back(ldVec2(1460,643));
            l.points.push_back(ldVec2(1417,716));
            l.points.push_back(ldVec2(1608,1046));
            l.points.push_back(ldVec2(1442,1046));
            l.points.push_back(ldVec2(1402,1120));
            l.points.push_back(ldVec2(1736,1120));
        }

        l.points.push_back(ldVec2(1460,643));

        float s = 1.0f/800.0f;
        float cx = 1400;
        float cy = 930;
        for (ldVec2 &p : l.points) {
            p.x -= cx;
            p.y -= cy;
            p.x *= s;
            p.y *= -s;
        }

        lines.append(l);
    }

    /*
         1402,532
         1068,1120
         1369.1120
         1414.1046
         1201.1046
         1446.616

         1460,643
         1417,716
         1608,1046
         1442,1046
         1402,1120
         1736,1220

         */
}
