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
#include <QtCore/QMutex>
#include <ldCore/Helpers/Maths/ldVec2.h>
#include <ldCore/Helpers/Maths/ldVec3.h>

#ifdef LD_CORE_KEYSTONE_CORRECTION
#ifdef Q_OS_LINUX
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core/utility.hpp>
#else // Q_OS_LINUX
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#endif
#endif

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

#ifdef LD_CORE_KEYSTONE_CORRECTION
    bool isNullKeystoneTransform() const;

    void setTopLeftXKeystone(float topLeftXValue);
    void setTopLeftYKeystone(float topLeftYValue);
    void setTopRightXKeystone(float topRightXValue);
    void setTopRightYKeystone(float topRightYValue);
    void setBottomLeftXKeystone(float bottomLeftXValue);
    void setBottomLeftYKeystone(float bottomLeftYValue);
    void setBottomRightXKeystone(float bottomRightXValue);
    void setBottomRightYKeystone(float bottomRightYValue);
#endif
    ldVec2 applyCornerKeystone(float x, float y);

private:
    void calcPitchYawCache();
    void calcMaxDim();
    void calcV2();
#ifdef LD_CORE_KEYSTONE_CORRECTION
    void calcHomographyCache();
#endif

    // params
    float beamAngleDeg = 21.04f;
    float m_yaw = 0;
    float m_pitch = 0;

#ifdef LD_CORE_KEYSTONE_CORRECTION
    float m_topLeftXKeystone = 0;
    float m_topLeftYKeystone = 1;
    float m_topRightXKeystone = 1;
    float m_topRightYKeystone = 1;
    float m_bottomLeftXKeystone = 0;
    float m_bottomLeftYKeystone = 0;
    float m_bottomRightXKeystone = 1;
    float m_bottomRightYKeystone = 0;
#endif

    // cache
    // beamAngleDeg only
    float m_dist = 0;
    // pitch yaw
    float m_maxdim = 0;
    ldVec3 m_v2;

#ifdef LD_CORE_KEYSTONE_CORRECTION
    cv::Mat m_homographyGrid;
#endif

    mutable QMutex m_mutex;
};


#endif // LDPROJECTIONUTIL

