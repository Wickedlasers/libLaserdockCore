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

#ifndef ldFilterBasicData_H
#define ldFilterBasicData_H

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QByteArray>

#include "ldCore/Render/ldRendererOpenlase.h"
#include "ldCore/ldCore_global.h"
#include "ldCore/Utilities/ldBasicDataStructures.h"
#include "ldBasicFilters.h"

class ldDeadzoneFilter;
class ldProjectionBasic;

// disables projector-oriented settings such as x/y size, position, etc.
#define FRAME_MODE_SKIP_TRANSFORM 0x01
// Disables the burn safety, which normally prevents the laser from focusing on a
// single point for too much time. Don't use this unless you know what you're doing.
#define FRAME_MODE_UNSAFE_UNDERSCAN 0x02
// Disables the galvanometer safety, which normally prevents sudden large movements in
// in x/y position that can damage the projector's scanner mechanism. Don't use this unless you know what you're doing.
#define FRAME_MODE_UNSAFE_OVERSCAN 0x04
// ignores app color and brightness settings.
#define FRAME_MODE_DISABLE_COLOR_CORRECTION 0x08

#define LD_DEFAULT_OFFSET 4

/** Filters that are applied to final data right before it is sent to laser */
class LDCORESHARED_EXPORT ldFilterBasicData: public ldFilter
{
public:
    /** Constructor/destructor */
    explicit ldFilterBasicData();
    virtual ~ldFilterBasicData();

    void addFilter(ldFilter *filter);

    /** ldFilter impl */
    virtual void process(Vertex &v) override;
    
    /** filter classes */
    ldColorCurveFilter *colorCurveFilter() const;
    ldDeadzoneFilter *deadzone() const;
    ldScaleFilter *scaleFilter() const;
    ldShiftFilter *shiftFilter() const;

    // keystone
    void setKeystoneX(float keystoneX);
    void setKeystoneY(float keystoneY);

    // alignment
    int m_offset = LD_DEFAULT_OFFSET;

    // color
    float m_brightness = 1.f;

    // scan protection
    bool galvo_libre = false;
    bool alternate_maxspeed = false;
    float underscan_speed = 0.003f;
    float overscan_speed = 0.120f;

    // frame mode flats
    int frameModes = 0;

private:
    static const int OFFSET_MAX = 10;
    Vertex old[OFFSET_MAX];

    std::unique_ptr<ldDeadzoneFilter> m_borderFilter;
    std::unique_ptr<ldColorCurveFilter> m_colorCurveFilter;
    std::unique_ptr<ldDeadzoneFilter> m_deadzoneFilter;
    std::unique_ptr<ldScaleFilter> m_scaleFilter;
    std::unique_ptr<ldShiftFilter> m_shiftFilter;

    std::unique_ptr<ldProjectionBasic> m_projectionBasic;
    std::vector<ldFilter*> m_filters;
};

#endif // ldFilterBasicData_H
