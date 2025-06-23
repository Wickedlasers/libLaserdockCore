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

#ifndef LDHARDWAREFILTER_H
#define LDHARDWAREFILTER_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

#include "ldFilterBasicGlobal.h"
#include "ldFilterBasicData.h"

class ldFilter;

template <typename T, typename Total, size_t N>
class Moving_Average;
class ldLaserPowerLimiter;

class LDCORESHARED_EXPORT ldHardwareFilter : public QObject
{
    Q_OBJECT

public:
    explicit ldHardwareFilter(ldScaleFilter *globalScaleFilter, QObject *parent = nullptr);
    ~ldHardwareFilter();

    void resetFilters();

    ldColorFilter *colorFilter() const;
    ldColorCurveFilter *baseColorCurveFilter() const;
    ldDeadzoneFilter *deadzone() const;
    ldFlipFilter *flipFilter();
    ldPowerFilter *powerFilter() const;
    ldRotateFilter *rotateFilter() const;
    ldScaleFilter *scaleFilter() const;
    ldShiftFilter *shiftFilter() const;
    ldTtlFilter *ttlFilter() const;

    // keystone
    void setKeystoneX(float keystoneX);
    void setKeystoneY(float keystoneY);

#ifdef LD_CORE_KEYSTONE_CORRECTION
    void setTopLeftXKeystone(float topLeftXValue);
    void setTopLeftYKeystone(float topLeftYValue);
    void setTopRightXKeystone(float topRightXValue);
    void setTopRightYKeystone(float topRightYValue);
    void setBottomLeftXKeystone(float bottomLeftXValue);
    void setBottomLeftYKeystone(float bottomLeftYValue);
    void setBottomRightXKeystone(float bottomRightXValue);
    void setBottomRightYKeystone(float bottomRightYValue);
#endif

    void setOffset(int offset);

    void processSafeLaserOutput(ldVertex &v);

    // alignment
    int m_offset = LD_DEFAULT_OFFSET;

    // scan protection
    bool galvo_libre = false;
    bool alternate_maxspeed = false;
    float underscan_speed = 0.003f;
    float overscan_speed = 0.120f;
    // They were static one..
    float m_lastX1 = 0;
    float m_lastY1 = 0;
    float m_lastX2 = 0;
    float m_lastY2 = 0;
    float m_averaged = 0;
    float m_turns = 0;

    static const int OFFSET_MAX = 10;
    ldVertex old[OFFSET_MAX];

    void processFrame(const ldVertexFrame &frame);
    void processVertex(ldVertex &v);
    void resetFilter();

    ldVertexFrame &lastFrame();

public slots:
    void setActive(bool isActive);
signals :

    void frameProcessed(ldVertexFrame frame);
    void deviceTemperatureUpdated(int tempDegC);

protected:

private:
    void processFrameV(ldVertex &v);

    static const size_t galvo_limiter_num_samples = 1200;

    ldVertexFrame m_lastFrame;
    std::unique_ptr<Moving_Average<float,double,galvo_limiter_num_samples>> m_galvoAverager;
    float m_scalelimiter{1.0f};
    bool m_isActive{false};
    std::unique_ptr<ldDeadzoneFilter> m_borderFilter;
    std::unique_ptr<ldColorFilter> m_colorFilter;
    std::unique_ptr<ldColorCurveFilter> m_colorCurveFilter;
    std::unique_ptr<ldDeadzoneFilter> m_deadzoneFilter;
    std::unique_ptr<ldFlipFilter> m_flipFilter;
    std::unique_ptr<ldPowerFilter> m_powerFilter;
    std::unique_ptr<ldProjectionBasic> m_projectionBasic;
    std::unique_ptr<ldRotateFilter> m_rotateFilter;
    std::unique_ptr<ldScaleFilter> m_scaleFilter;
    std::unique_ptr<ldShiftFilter> m_shiftFilter;
    std::unique_ptr<ldTtlFilter> m_ttlFilter;
    std::unique_ptr<ldLaserPowerLimiter> m_laserPowerFilter;

signals:
    void activeChanged(bool isActive);
};

#endif // LDHARDWAREFILTER_H
