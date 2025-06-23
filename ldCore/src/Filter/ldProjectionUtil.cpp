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

#ifdef LD_CORE_KEYSTONE_CORRECTION
void ldProjectionBasic::setTopLeftXKeystone(float topLeftXValue)
{
    if (topLeftXValue > m_topRightXKeystone) return;
    m_topLeftXKeystone = topLeftXValue;
    calcHomographyCache();
}
void ldProjectionBasic::setTopLeftYKeystone(float topLeftYValue)
{
    if (topLeftYValue < m_bottomLeftYKeystone) return;
    m_topLeftYKeystone = topLeftYValue;
    calcHomographyCache();
}

void ldProjectionBasic::setTopRightXKeystone(float topRightXValue)
{
    if (topRightXValue < m_topLeftXKeystone) return;
    m_topRightXKeystone = topRightXValue;
    calcHomographyCache();
}
void ldProjectionBasic::setTopRightYKeystone(float topRightYValue)
{
    if (topRightYValue < m_bottomRightYKeystone) return;
    m_topRightYKeystone = topRightYValue;
    calcHomographyCache();
}

void ldProjectionBasic::setBottomLeftXKeystone(float bottomLeftXValue)
{
    if (bottomLeftXValue > m_bottomRightXKeystone) return;
    m_bottomLeftXKeystone = bottomLeftXValue;
    calcHomographyCache();
}
void ldProjectionBasic::setBottomLeftYKeystone(float bottomLeftYValue)
{
    if (bottomLeftYValue > m_topLeftYKeystone) return;
    m_bottomLeftYKeystone = bottomLeftYValue;
    calcHomographyCache();
}

void ldProjectionBasic::setBottomRightXKeystone(float bottomRightXValue)
{
    if (bottomRightXValue < m_bottomLeftXKeystone) return;
    m_bottomRightXKeystone = bottomRightXValue;
    calcHomographyCache();
}
void ldProjectionBasic::setBottomRightYKeystone(float bottomRightYValue)
{
    if (bottomRightYValue > m_topRightYKeystone) return;
    m_bottomRightYKeystone = bottomRightYValue;
    calcHomographyCache();
}

#endif

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

#ifdef LD_CORE_KEYSTONE_CORRECTION
bool ldProjectionBasic::isNullKeystoneTransform() const
{
    return cmpf(m_topLeftXKeystone, 0) && cmpf(m_topLeftYKeystone, 1)
             && cmpf(m_topRightXKeystone, 1) && cmpf(m_topRightYKeystone, 1)
            && cmpf(m_bottomLeftXKeystone, 0) && cmpf(m_bottomLeftYKeystone, 0)
            && cmpf(m_bottomRightXKeystone, 1) && cmpf(m_bottomRightYKeystone, 0);
}
#endif

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

#ifdef LD_CORE_KEYSTONE_CORRECTION
//
void ldProjectionBasic::calcHomographyCache()
{
    // from top left to bottom left, clockwise
    std::vector<cv::Point2f> keyStonePoints;
    keyStonePoints.push_back(cv::Point2f(m_topLeftXKeystone*2.0f - 1.0f, m_topLeftYKeystone*2.0f - 1.0f));
    keyStonePoints.push_back(cv::Point2f(m_topRightXKeystone*2.0f - 1.0f, m_topRightYKeystone*2.0f - 1.0f));
    keyStonePoints.push_back(cv::Point2f(m_bottomRightXKeystone*2.0f - 1.0f, m_bottomRightYKeystone*2.0f - 1.0f));
    keyStonePoints.push_back(cv::Point2f(m_bottomLeftXKeystone*2.0f - 1.0f, m_bottomLeftYKeystone*2.0f - 1.0f));

    //qDebug()<<"m_topLeftXKeystone*2.0f - 1.0f "<< (m_topLeftXKeystone*2.0f - 1.0f) << "  m_topLeftYKeystone*2.0f - 1.0f" << (m_topLeftYKeystone*2.0f - 1.0f);
    //qDebug()<<"m_topRightXKeystone*2.0f - 1.0f "<< (m_topRightXKeystone*2.0f - 1.0f) << "  m_topRightYKeystone*2.0f - 1.0f" << (m_topRightYKeystone*2.0f - 1.0f);
    //qDebug()<<"m_bottomRightXKeystone*2.0f - 1.0f "<< (m_bottomRightXKeystone*2.0f - 1.0f) << "  m_bottomRightYKeystone*2.0f - 1.0f" << ( m_bottomRightYKeystone*2.0f - 1.0f);
    //qDebug()<<"m_bottomLeftXKeystone*2.0f - 1.0f "<< (m_bottomLeftXKeystone*2.0f - 1.0f) << "  m_bottomLeftYKeystone*2.0f - 1.0f" << (m_bottomLeftYKeystone*2.0f - 1.0f);
    //
    std::vector<cv::Point2f> corners_th;
    corners_th.push_back(cv::Point2f(-1.0f, 1.0f));
    corners_th.push_back(cv::Point2f(1.0f, 1.0f));
    corners_th.push_back(cv::Point2f(1.0f, -1.0f));
    corners_th.push_back(cv::Point2f(-1.0f, -1.0f));
    //
    QMutexLocker lock(&m_mutex);
    m_homographyGrid = cv::findHomography(corners_th, keyStonePoints, cv::RANSAC);

    //
    //3   libsystem_malloc.dylib        	    0x7ff81eabd3e2 malloc_vreport + 548
    //4   libsystem_malloc.dylib        	    0x7ff81eac05ed malloc_report + 151
    //5   libopencv_core.3.4.18.dylib   	       0x1123f851e cv::StdMatAllocator::deallocate(cv::UMatData*) const + 46
    //6   libopencv_core.3.4.18.dylib   	       0x1123f002e cv::Mat::~Mat() + 78
}

// applyCornerKeystone
ldVec2 ldProjectionBasic::applyCornerKeystone(float x, float y)
{
    QMutexLocker lock(&m_mutex);

    std::vector<cv::Point2f> fromPoint;
    std::vector<cv::Point2f> toPoint;
    //qDebug() << " m_homographyGrid rows" <<  m_homographyGrid.rows;
    //qDebug() << " m_homographyGrid cols" <<  m_homographyGrid.cols;
    if (m_homographyGrid.rows != 3 || m_homographyGrid.cols != 3) return ldVec2(x, y);
    //
    fromPoint.push_back(cv::Point2f(x, y));
    cv::perspectiveTransform(fromPoint, toPoint, m_homographyGrid);
    //qDebug() << " x" << x << "   --> " << toPoint[0].x;
    return ldVec2(toPoint[0].x, toPoint[0].y);
}
#endif
