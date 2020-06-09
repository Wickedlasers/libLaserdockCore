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

#include <ldCore/Helpers/Maths/ldVec3.h>

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

    bool isNullTransform() const;

private:
    void calcPitchYawCache();
    void calcMaxDim();
    void calcV2();

    // params
    float beamAngleDeg = 21.04f;
    float m_yaw = 0;
    float m_pitch = 0;

    // cache
    // beamAngleDeg only
    float m_dist = 0;
    // pitch yaw
    float m_maxdim = 0;
    ldVec3 m_v2;
};


#endif // LDPROJECTIONUTIL

