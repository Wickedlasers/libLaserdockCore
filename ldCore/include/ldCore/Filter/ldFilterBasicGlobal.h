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

#ifndef ldFilterBasicGlobal_H
#define ldFilterBasicGlobal_H

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QByteArray>

#include "ldCore/Render/ldRendererOpenlase.h"
#include "ldCore/ldCore_global.h"

#include "ldBasicFilters.h"
#include "ldSoundLevelFilter.h"


/** Filters that are applied to Vertex before it is sent to simulator */
class LDCORESHARED_EXPORT ldFilterBasicGlobal: public ldFilter {
public:
    /** Constructor/destructor */
    explicit ldFilterBasicGlobal();
    virtual ~ldFilterBasicGlobal();

    /** ldFilter impl */
    virtual void process(ldVertex &v) override;

    /** Filters */
    ldColorCurveFilter *colorCurveFilter() const;
    ldColorFaderFilter *colorFaderFilter() const;
    ldStrobeFilter *strobeFilter() const;
    ldHueFilter *hueFilter() const;
    ldHueMatrixFilter *hueMatrixFilter() const;
    ldHueShiftFilter *hueShiftFilter() const;
    ldSoundLevelFilter *soundLevelFilter() const;
    ldTracerFilter *tracerFilter() const;

    void setHueFiltersActive(bool active);

private:
    std::unique_ptr<ldColorCurveFilter> m_colorCurveFilter;
    std::unique_ptr<ldColorFaderFilter> m_colorFaderFilter;
    std::unique_ptr<ldStrobeFilter> m_strobeFilter;
    std::unique_ptr<ldHueFilter> m_hueFilter;
    std::unique_ptr<ldHueMatrixFilter> m_hueMatrixFilter;
    std::unique_ptr<ldHueShiftFilter> m_hueShiftFilter;
    std::unique_ptr<ldSoundLevelFilter> m_soundLevelFilter;
    std::unique_ptr<ldTracerFilter> m_tracerFilter;

    bool m_isHueFiltersActive = true;
};



#endif // ldFilterBasicGlobal_H
