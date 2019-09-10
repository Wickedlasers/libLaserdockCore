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

#ifndef LDPROJECTIONUTIL
#define LDPROJECTIONUTIL

//#include <QtCore/qglobal.h>

#include "ldCore/ldCore_global.h"

class LDCORESHARED_EXPORT vt3  {
public:
    vt3(float p_x = 0, float p_y = 0, float p_z = 0);

    void rot(const vt3 &k, float theta);

    float x = 0;
    float y = 0;
    float z = 0;

private:
    void sm(float f);
    float dot(const vt3 &b) const;
    void cross(const vt3 &b);
    void add(const vt3 &b);
};

class LDCORESHARED_EXPORT ldProjectionBasic
{
public:
    ldProjectionBasic();
    ~ldProjectionBasic();

    void setPitch(float pitch);
    float pitch() const;

    void setYaw(float yaw);
    float yaw() const;

    void transform (float &x, float &y);
    float maxdim() const;

private:
    void calcPitchYawCache();
    void calcMaxDim();
    void calcV2();

    // params
    float beamAngleDeg = 21.04f;
    float projectorYaw = 0;
    float projectorPitch = 0;

    // cache
    // beamAngleDeg only
    float m_dist = 0;
    // pitch yaw
    float m_maxdim = 0;
    vt3 m_v2;
};


#endif // LDPROJECTIONUTIL

