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

#include <ldCore/Filter/ldProjectionUtil.h>

#include <cmath>

#include <ldCore/Helpers/Maths/ldGlobals.h>

ldProjectionBasic::ldProjectionBasic()
{
    m_dist = sinf(beamAngleDeg * M_PIf / 180.0f);
    if (m_dist > 0) m_dist = 1.0f / m_dist;

    calcPitchYawCache();
}

ldProjectionBasic::~ldProjectionBasic()
{
}

void ldProjectionBasic::setPitch(float pitch)
{
    if(cmpf(m_pitch, pitch))
        return;

    m_pitch = pitch;

    calcPitchYawCache();
}

float ldProjectionBasic::pitch() const
{
    return m_pitch;
}

void ldProjectionBasic::setYaw(float yaw)
{
    if(cmpf(m_yaw, yaw))
        return;

    m_yaw = yaw;

    calcPitchYawCache();
}

float ldProjectionBasic::yaw() const
{
    return m_yaw;
}

void ldProjectionBasic::transform (float &x, float &y) {
    ldVec3 v3 = m_v2;
    v3.x += x;
    v3.y += y;

    v3.rotate(ldVec3::Y_VECTOR, -m_yaw);
    v3.rotate(ldVec3::X_VECTOR, -m_pitch);

    float tz = 0.01f;
    if (v3.z > tz) tz = v3.z;

    x = v3.x/tz*m_dist;
    y = v3.y/tz*m_dist;

    x = v3.x;
    y = v3.y;
}

float ldProjectionBasic::maxdim() const
{
    return m_maxdim;
}

bool ldProjectionBasic::isNullTransform() const
{
    return cmpf(m_yaw, 0) && cmpf(m_pitch, 0);
}

void ldProjectionBasic::calcPitchYawCache()
{
    calcV2();
    calcMaxDim();
}

void ldProjectionBasic::calcMaxDim()
{
    //float dimx = maxx-minx;
    //float dimy = maxy-miny;
    //float maxdim = (dimx > dimy) ? dimx : dimy;

    // calculate size of projectable area
//        float cx = 0, cy = 0;
//        m_map->transform(cx, cy);
    float minx = 0, maxx = 0, miny = 0, maxy = 0;
    float maxdim = 0;

    for (int k = 0; k < 4; k++) {
        float tx = (k%2)?-1.f:1.f;
        float ty = (k/2)?-1.f:1.f;
        transform(tx, ty);

        if (tx < minx) minx = tx;
        if (tx > maxx) maxx = tx;
        if (ty < miny) miny = ty;
        if (ty > maxy) maxy = ty;
        if (tx > maxdim) maxdim = tx;
        if (-tx > maxdim) maxdim = -tx;
        if (ty > maxdim) maxdim = ty;
        if (-ty > maxdim) maxdim = -ty;
    }

    m_maxdim = maxdim;
}

void ldProjectionBasic::calcV2()
{
    ldVec3 v0;
    v0.z = 1;

    v0.rotate(ldVec3::X_VECTOR, m_pitch);
    ldVec3 &v1 = v0;
    v1.x *= m_dist/v1.z;
    v1.y *= m_dist/v1.z;
    v1.z = m_dist;
    v1.rotate(ldVec3::Y_VECTOR, m_yaw);
    ldVec3 &v2 = v1;
    v2.x *= m_dist/v2.z;
    v2.y *= m_dist/v2.z;
    v2.z = m_dist;

    m_v2 = v2;
}
