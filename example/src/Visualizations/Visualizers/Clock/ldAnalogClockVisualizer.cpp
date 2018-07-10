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

#include "ldAnalogClockVisualizer.h"

#include "ldClockComplexObject.h"

ldAnalogClockVisualizer::ldAnalogClockVisualizer(QObject *parent)
    : ldVisualizer(parent)
    , m_clockObj(new ldClockComplexObject)
{
    m_isMusicAware = false;

    init();
}

ldAnalogClockVisualizer::~ldAnalogClockVisualizer()
{
}

bool ldAnalogClockVisualizer::init()
{
    if (ldVisualizer::init())
    {
        setPosition(ccp(1, 1));
        return true;
    }
    return false;
}

void ldAnalogClockVisualizer::onShouldStart()
{
    OLRenderParams params;
    memset(&params, 0, sizeof params);
    params.rate = m_renderer->rate();
    params.on_speed = 2.0f / 45.0f;
    params.off_speed = 2.0f / 45.0f;
    params.start_wait = 4;
    params.start_dwell = 2;
    params.curve_dwell = 1;
    params.corner_dwell = 3;
    params.curve_angle = cosf(30.0f*(M_PIf / 180.0f)); // 30 deg
    params.end_dwell = 2;
    params.end_wait = 2;
    params.snap = 1 / 1000.0f;
    params.render_flags = RENDER_NOREVERSE | RENDER_NOREORDER; // | RENDER_GRAYSCALE
    params.flatness = 0;
    params.min_length = 0;
    params.max_framelen = params.rate / (targetFPS() - 5);
    m_renderer->setRenderParams(&params);
}

void ldAnalogClockVisualizer::draw()
{
    m_clockObj->innerDraw(m_renderer);
}
