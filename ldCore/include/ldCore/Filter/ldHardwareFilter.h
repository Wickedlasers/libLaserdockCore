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

class LDCORESHARED_EXPORT ldHardwareFilter : public QObject
{
    Q_OBJECT

public:
    explicit ldHardwareFilter(ldScaleFilter *globalScaleFilter, QObject *parent = nullptr);
    ~ldHardwareFilter();

    void resetFilters();

    ldColorCurveFilter *baseColorCurveFilter() const;
    ldDeadzoneFilter *deadzone() const;
    ldFlipFilter *flipFilter();
    ldPowerFilter *powerFilter() const;
    ldScaleFilter *scaleFilter() const;
    ldShiftFilter *shiftFilter() const;
    ldTtlFilter *ttlFilter() const;

    // keystone
    void setKeystoneX(float keystoneX);
    void setKeystoneY(float keystoneY);
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
protected:

private:
    void processFrameV(ldVertex &v);

    ldVertexFrame m_lastFrame;

    std::unique_ptr<ldDeadzoneFilter> m_borderFilter;
    std::unique_ptr<ldColorCurveFilter> m_colorCurveFilter;
    std::unique_ptr<ldDeadzoneFilter> m_deadzoneFilter;
    std::unique_ptr<ldFlipFilter> m_flipFilter;
    std::unique_ptr<ldPowerFilter> m_powerFilter;
    std::unique_ptr<ldProjectionBasic> m_projectionBasic;
    std::unique_ptr<ldScaleFilter> m_scaleFilter;
    std::unique_ptr<ldShiftFilter> m_shiftFilter;
    std::unique_ptr<ldTtlFilter> m_ttlFilter;

};

#endif // LDHARDWAREFILTER_H
