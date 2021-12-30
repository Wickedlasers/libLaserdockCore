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

// ignore 3d rotate
#define FRAME_MODE_DISABLE_ROTATION 0x10

#define LD_DEFAULT_OFFSET 4
#define LD_DEFAULT_PRO_OFFSET 2

/** Filters that are applied to final data right before it is sent to laser */
class LDCORESHARED_EXPORT ldFilterBasicData: public ldFilter
{
public:
    /** Constructor/destructor */
    explicit ldFilterBasicData();
    virtual ~ldFilterBasicData();

    /** ldFilter impl */
    virtual void process(ldVertex &v) override;

    /** filter classes */
    ldRotateFilter *rotateFilter() const;
    ldScaleFilter *scaleFilter() const;
    ldPowerFilter *powerFilter() const;


    // frame mode flats
    int frameModes = 0;

private:
    std::unique_ptr<ldRotateFilter> m_rotateFilter;
    std::unique_ptr<ldScaleFilter> m_scaleFilter;
    std::unique_ptr<ldPowerFilter> m_powerFilter;
};

#endif // ldFilterBasicData_H
