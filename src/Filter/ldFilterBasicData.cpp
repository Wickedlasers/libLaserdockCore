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

#include "ldCore/Filter/ldFilterBasicData.h"

#include <math.h>

#include <QtCore/QDebug>

#include "ldCore/Filter/ldFilterBasicGlobal.h"

#include "ldCore/Filter/ldDeadzoneFilter.h"
#include "ldCore/Filter/ldProjectionUtil.h"

ldFilterBasicData::ldFilterBasicData()
    : ldFilter()
    , m_borderFilter(new ldDeadzoneFilter())
    , m_colorCurveFilter(new ldColorCurveFilter())
    , m_deadzoneFilter(new ldDeadzoneFilter())
    , m_scaleFilter(new ldScaleFilter())
    , m_shiftFilter(new ldShiftFilter(m_scaleFilter.get()))
    , m_projectionBasic(new ldProjectionBasic)
{
    m_colorCurveFilter->m_enabled = true;

    // set up deadzone with default zone
    m_deadzoneFilter->resetToDefault();
    // set up border
    ldDeadzoneFilter::Deadzone deadzone;
    deadzone.m_attenuation = 1;
    deadzone.m_rect.setWidth(1.01f);
    deadzone.m_rect.setHeight(1.01f);
    deadzone.m_rect.moveLeft(0);
    deadzone.m_rect.moveTop(0);
    m_borderFilter->add(deadzone);
    m_borderFilter->m_enabled = true;
    m_borderFilter->m_reverse = true;
    
    // data for offset correction
    // init old[] buffer with zeroes
    Vertex t;
    t.position[0] = t.position[1] = t.position[2] = 0;
    t.color[0] = t.color[1] = t.color[2] = t.color[3] = 0;
    for (int i = 0; i < OFFSET_MAX; i++) old[i] = t;
}

ldFilterBasicData::~ldFilterBasicData()
{
}

void ldFilterBasicData::addFilter(ldFilter *filter)
{
    m_filters.push_back(filter);
}

void ldFilterBasicData::process(Vertex &v) {
    
    for(ldFilter *filter : m_filters)
        filter->process(v);

    bool mode_disable_scale = frameModes & FRAME_MODE_SKIP_TRANSFORM;
    bool mode_disable_projection = frameModes & FRAME_MODE_SKIP_TRANSFORM;
    bool mode_disable_underscan = frameModes & FRAME_MODE_UNSAFE_UNDERSCAN;
    bool mode_disable_overscan = frameModes & FRAME_MODE_UNSAFE_OVERSCAN;
    bool mode_disable_colorcorrection = frameModes & FRAME_MODE_DISABLE_COLOR_CORRECTION;

    if (!mode_disable_scale) {
        // scale
        m_scaleFilter->process(v);
        m_shiftFilter->process(v);
    }

    // keystone
    if (!mode_disable_projection) {
        bool nullTransform = (m_projectionBasic->pitch() == 0 && m_projectionBasic->yaw() == 0);
        if(!nullTransform) {
            // map
            float x = v.position[0];
            float y = v.position[1];
            // correct for size
            x *= 1.0f/m_projectionBasic->maxdim();
            y *= 1.0f/m_projectionBasic->maxdim();
            // transform
            m_projectionBasic->transform(x, y);

            v.position[0] = x;
            v.position[1] = y;
        }
    }

    // borders and clamp
    m_borderFilter->process(v);
    v.position[0] = std::min<float>(std::max<float>(v.position[0], -1), 1);
    v.position[1] = std::min<float>(std::max<float>(v.position[1], -1), 1);

    // deadzone
    // disable rotate before applying deadzone
    m_deadzoneFilter->process(v);

    // global brightness 1
    v.color[0] *= m_brightness;
    v.color[1] *= m_brightness;
    v.color[2] *= m_brightness;

    // apply color curves
    if (!mode_disable_colorcorrection) {
        for (int i = 0; i < 3; i++)
            v.color[i] = fminf(fmaxf(v.color[i], 0), 1);

        m_colorCurveFilter->process(v);

        for (int i = 0; i < 3; i++)
            v.color[i] = fminf(fmaxf(v.color[i], 0), 1);
    }

    // scan protection
    if (!galvo_libre) {
        
        // scan protection
        // overscan
        // quiet algorithm
        if (!mode_disable_overscan) {
            float maxDistance = overscan_speed; // (per point)
            float tx = v.position[0];
            float ty = v.position[1];
            static float lastx = 0;
            static float lasty = 0;
            float dx = tx - lastx;
            float dy = ty - lasty;
            float dist = sqrtf(dx*dx+dy*dy);
            if (!alternate_maxspeed) {
                // algorithm with a max 2d distance.  has fewer visual artifacts but is more limiting
                float mult = 1.0;
                if (dist > maxDistance) mult = maxDistance / dist;
                tx = lastx + dx*mult;
                ty = lasty + dy*mult;
            } else {
                // alternate algorithm treats x/y galvos independantly
                if (dx > maxDistance) dx = maxDistance;
                if (dx < -maxDistance) dx = -maxDistance;
                if (dy > maxDistance) dy = maxDistance;
                if (dy < -maxDistance) dy = -maxDistance;
                tx = lastx + dx;
                ty = lasty + dy;
            }
            v.position[0] = lastx = tx;
            v.position[1] = lasty = ty;
            
        }
        
        // underscan
        // restless algorithm
        // if laser is too slow it will draw a circle
        bool underscan = true;
        // skip if it's a black point
        if (v.color[0] == 0 && v.color[1] == 0 && v.color[2] == 0) underscan = false;
        if (underscan && !mode_disable_underscan)
        {
            static float lastx = 0;
            static float lasty = 0;
            float dx = v.position[0] - lastx;
            float dy = v.position[1] - lasty;
            float d = sqrtf(dx*dx+dy*dy);
            if (d > underscan_speed) {
                d = 1;
                lastx = v.position[0];
                lasty = v.position[1];
            } else {
                d = 0;
            }
            //if (v.color[0] == 0 && v.color[1] == 0 && v.color[2] == 0) d = 1;
            // with these constants, algo takes about 2ms of constant laser to activate
            static float averaged = 0;
            averaged = 0.99*averaged + 0.01*d;
            if (d == 1) averaged = 0.98*averaged + 0.02*d;
            float r = 0;
            float mad = 0.4f; // at least 40% of points must be under
            if (d == 0 && averaged < mad) r = 0.125 * (mad-averaged)/mad;
            
            
            if (r > 0) {
                // move laser in a circle and fade out
                static float turns = 0;
                turns += 1.0/64.0;
                turns -= (int) turns;
                // set color
                float ff = fminf(fmaxf(1.0,((mad-averaged)/mad)*2-1), 1);
                v.color[0] *= 1-ff;
                v.color[1] *= 1-ff;
                v.color[2] *= 1-ff;
            }
        }
        
    }
   

	//
	// color/pos alignment offset
	// save in old buffer in praparation for performing offset
	for (int i = OFFSET_MAX - 1; i >= 1; i--)
		old[i] = old[i - 1];
	old[0] = v;

	// perform offset
	if (m_offset < -OFFSET_MAX + 1) m_offset = -OFFSET_MAX + 1;
	if (m_offset >  OFFSET_MAX - 1) m_offset = OFFSET_MAX - 1;
	// convert signed offset value to array indexes coloroffset and posoffset
	int co = 0;
	int po = 0;
	if (m_offset > 0) co = m_offset;
	if (m_offset < 0) po = -m_offset;
	// take values from old buffer depending on offsets
	for (int i = 0; i < 4; i++) v.color[i] = old[co].color[i];
	for (int i = 0; i < 3; i++) v.position[i] = old[po].position[i];
}

ldColorCurveFilter *ldFilterBasicData::colorCurveFilter() const
{
    return m_colorCurveFilter.get();
}

ldDeadzoneFilter *ldFilterBasicData::deadzone() const
{
    return m_deadzoneFilter.get();
}

ldScaleFilter *ldFilterBasicData::scaleFilter() const
{
    return m_scaleFilter.get();
}

ldShiftFilter *ldFilterBasicData::shiftFilter() const
{
    return m_shiftFilter.get();
}

void ldFilterBasicData::setKeystoneX(float keystoneX)
{
    m_projectionBasic->setPitch(keystoneX);
}

void ldFilterBasicData::setKeystoneY(float keystoneY)
{
    m_projectionBasic->setYaw(keystoneY);
}
