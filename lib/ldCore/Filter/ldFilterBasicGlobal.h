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
#include "ldCore/Utilities/ldBasicDataStructures.h"

#include "ldBasicFilters.h"

class LDCORESHARED_EXPORT ldFilterBasicGlobal: public ldFilter {
public:
    explicit ldFilterBasicGlobal();
    virtual ~ldFilterBasicGlobal();

    virtual void process(Vertex &v);

    ldColorCurveFilter *colorCurveFilter() const;
    ldHueFilter *hueFilter() const;
    ldHueMatrixFilter *hueMatrixFilter() const;
    ldHueShiftFilter *hueShiftFilter() const;
    ldRotateFilter *rotateFilter() const;
    ldTracerFilter *tracerFilter() const;

    bool ttl = false;
    bool flipX = false;
    bool flipY = false;

private:
    std::unique_ptr<ldColorCurveFilter> m_colorCurveFilter;
    std::unique_ptr<ldHueFilter> m_hueFilter;
    std::unique_ptr<ldHueMatrixFilter> m_hueMatrixFilter;
    std::unique_ptr<ldHueShiftFilter> m_hueShiftFilter;
    std::unique_ptr<ldRotateFilter> m_rotateFilter;
    std::unique_ptr<ldTracerFilter> m_tracerFilter;

    ldColorCurve curveR, curveG, curveB;
};



#endif // ldFilterBasicGlobal_H
