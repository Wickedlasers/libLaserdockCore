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

class LDCORESHARED_EXPORT ldDeadzoneFilter: public ldFilter {
    
public:
    class LDCORESHARED_EXPORT Deadzone {
    public:
        Deadzone(QRectF rect = QRectF(), float attenuation = 1.f);

        QRectF visRect() const; // rect in vis coordinates
        bool isValid() const;

        QRectF m_rect; // pos -1..1, size 0..1
        float m_attenuation = 1.0f;
    };

    explicit ldDeadzoneFilter();

    virtual void process(Vertex &v) override;

    void add(const Deadzone &deadzone);
    void clear();

    const QList<Deadzone> &deadzones() const;
    Deadzone *currentDeadzone();

    void  resetToDefault();

    // dead zone
    bool m_enabled = false;
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
