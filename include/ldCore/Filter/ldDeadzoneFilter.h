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

#ifndef LDDEADZONEFILTER_H
#define LDDEADZONEFILTER_H

#include "ldCore/ldCore_global.h"
#include "ldCore/Utilities/ldBasicDataStructures.h"
#include "ldFilter.h"

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QByteArray>
#include <QtCore/QRectF>

#include <ldCore/Render/ldRendererOpenlase.h>

/** The filter allows to attenuate output at/out of selected zones.
 *  If some zone intersect the other zone, the zone with the maximum attenuation value is choosed
 */
class LDCORESHARED_EXPORT ldDeadzoneFilter: public ldFilter {
    
public:
    /** Zone class */
    class LDCORESHARED_EXPORT Deadzone
    {
    public:
        Deadzone(QRectF rect = QRectF(), float attenuation = 1.f);

        /** Get rect in Vertex coordinates */
        QRectF visRect() const; // rect in vis coordinates

        QRectF m_rect; // pos -1..1, size 0..1
        float m_attenuation = 1.0f; // 1 - full block, 0 - no block
    };

    /** Constructor */
    explicit ldDeadzoneFilter();

    /** ldFilter impl */
    virtual void process(Vertex &v) override;

    /** Add new deadzone */
    void add(const Deadzone &deadzone);
    /** Remove all deadzones */
    void clear();

    /** List of deadzones */
    const QList<Deadzone> &deadzones() const;

    /** helper func to get pointer to the first deadzone. If there are no deadzones nullptr is returned */
    Deadzone *firstDeadzone();

    /** Creates default deadzone in the middle of screen */
    void resetToDefault();

    /** Enable filter flag */
    bool m_enabled = false;
    /** in/out filter control */
    bool m_reverse = false;

private:
    // for dead zone
    void attenuate(Vertex &v);
    bool isOutside(float x, float y) const;
    ldDeadzoneFilter::Deadzone getDeadzone(float x, float y) const;


    bool m_laston = false;
    Vertex m_lastVertex;

    QList<Deadzone> m_deadzones;
};

#endif // LDDEADZONEFILTER_H
