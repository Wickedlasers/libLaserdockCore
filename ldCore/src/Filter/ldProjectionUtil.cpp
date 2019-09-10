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

#include <cmath>

#include "ldCore/Filter/ldProjectionUtil.h"

// -------------------- vt3 -------------------------------

const static vt3 vright = vt3{1, 0, 0};
const static vt3 vup = vt3{0, 1, 0};
const static vt3 vfwd = vt3{0, 0, 1};


vt3::vt3(float p_x, float p_y, float p_z)
    : x(p_x)
    , y(p_y)
    , z(p_z)
{}

void vt3::sm(float f) {
    x *= f;
    y *= f;
    z *= f;
}

float vt3::dot(const vt3& b) const {
    return x*b.x + y*b.y + z*b.z;
}

void vt3::cross(const vt3& b)  {
    vt3 r = *this;
    x = r.y*b.z - r.z*b.y;
    y = r.z*b.x - r.x*b.z;
    z = r.x*b.y - r.y*b.x;
}

void vt3::add(const vt3& b) {
    x += b.x;
    y += b.y;
    z += b.z;
}

void vt3::rot(const vt3& k, float theta) {
    vt3 initialValue = *this;
    vt3 k1 = k;
    vt3 k2 = k;

    float thetaCos = cosf(theta);

    // a
    sm(thetaCos);

    // b
    k1.cross(initialValue);
    k1.sm(sinf(theta));
    add(k1);

    // c
    float cF = k2.dot(initialValue) * (1 - thetaCos);
    k2.sm(cF);
    add(k2);
}


// -------------------- ldProjectionBasic -------------------------------

ldProjectionBasic::ldProjectionBasic()
{
    m_dist = sinf(beamAngleDeg * (float) M_PI / 180.0f);
    if (m_dist > 0) m_dist = 1.0f / m_dist;

    calcPitchYawCache();
}

ldProjectionBasic::~ldProjectionBasic()
{
}

void ldProjectionBasic::setPitch(float pitch)
{
    if(projectorPitch == pitch)
        return;

    projectorPitch = pitch;

    calcPitchYawCache();
}

float ldProjectionBasic::pitch() const
{
    return projectorPitch;
}

void ldProjectionBasic::setYaw(float yaw)
{
    if(projectorYaw == yaw )
        return;

    projectorYaw = yaw;

    calcPitchYawCache();
}

float ldProjectionBasic::yaw() const
{
    return projectorYaw;
}

void ldProjectionBasic::transform (float &x, float &y) {
    vt3 v3 = m_v2;
    v3.x += x;
    v3.y += y;

    v3.rot(vup, -projectorYaw);
    vt3 &v4 = v3;
    v4.rot(vright, -projectorPitch);;
    vt3 &v5 = v4;

    vt3 &vz = v5;
    float tz = 0.01f;
    if (vz.z > tz) tz = vz.z;

    x = vz.x/tz*m_dist;
    y = vz.y/tz*m_dist;

}

float ldProjectionBasic::maxdim() const
{
    return m_maxdim;
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
    vt3 v0;
    v0.z = 1;

    v0.rot(vright, projectorPitch);
    vt3 &v1 = v0;
    v1.x *= m_dist/v1.z;
    v1.y *= m_dist/v1.z;
    v1.z = m_dist;
    v1.rot(vup, projectorYaw);
    vt3 &v2 = v1;
    v2.x *= m_dist/v2.z;
    v2.y *= m_dist/v2.z;
    v2.z = m_dist;

    m_v2 = v2;
}
