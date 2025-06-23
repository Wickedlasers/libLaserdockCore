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

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QByteArray>
#include <QtCore/QRectF>

#include "ldCore/ldCore_global.h"
#include <ldCore/Render/ldRendererOpenlase.h>
#include <ldCore/Helpers/Maths/ldVec2.h>

#include "ldFilter.h"


/** The filter allows to attenuate output at/out of selected zones.
 *  If some zone intersect the other zone, the zone with the maximum attenuation value is choosed
 */
class LDCORESHARED_EXPORT ldDeadzoneFilter: public ldFilter
{
    Q_OBJECT
public:

    enum DeadZoneShapes {
        MShapeRect,
        MShapeCircle,
        MShapeTriangle,
        MShapeCornerTopLeft,
        MShapeCornerTopRight,
        MShapeCornerBotRight,
        MShapeCornerBotLeft,
        MShapeDiamond,
        First = MShapeRect,
        Last = MShapeDiamond
    };
    Q_ENUM(DeadZoneShapes)

    /** Zone class */
    class LDCORESHARED_EXPORT Deadzone
    {
    public:

        Deadzone(QRectF rect = QRectF(), float attenuation = 1.f, int shapeIndex = 0);

        QRectF rect() const;
        /** Get rect in Vertex coordinates */
        const QRectF &visRect() const; // rect in vis coordinates
        const QList<QPointF> &shape() const; // shape in vis coordinates

        void moveLeft(float value);
        void moveTop(float value);
        void setWidth(float value);
        void setHeight(float value);

        bool contains(float x, float y) const;

        void setAttenuation(float attenuation);
        float attenuation() const;

        void setShapeIndex(int shapeIndex);
        int shapeIndex() const;

    private:
        void updateVisRect();
        void updateShapeRectangle();
        void updateShapeCircle();
        void updateShapeTriangle();
        void updateShapeCorner(int p_cornerCase);
        void updateShapeDiamond();

        float m_attenuation = 1.0f; // 1 - full block, 0 - no block
        int m_shapeIndex = 0; // 0 rectangle 1 circle
        QRectF m_rect; // pos -1..1, size 0..1
        QRectF m_visRect;
        QList<QPointF> m_shape; // in vis coordinates
    };

    /** Constructor */
    explicit ldDeadzoneFilter();

    /** process the whole frame if possible, works much better
     * ldFilter usage is deprecated and should be replaced in the future
     **/
    void processFrame(std::vector<ldVertex> &frame);

    /** ldFilter impl */
    virtual void process(ldVertex &v) override;
    virtual void resetFilter() override;

    /** Add new deadzone */
    void add(const Deadzone &deadzone);
    /** Remove all deadzones */
    void clear();
    void removeCurrent();

    /** List of deadzones */
    const QList<Deadzone> &deadzones() const;

    /** helper func to get pointer to the selected deadzone. If there are no deadzones nullptr is returned */
    Deadzone *selectedDeadzone();

    int selectedIndex() const;
    void setSelectedIndex(int index);

    /** Creates default deadzone in the middle of screen */
    void resetToDefault();

    /** in/out zone switch */
    void setReverse(bool reverse);

    /** Enable filter flag */
    void setEnabled(bool enabled);
    void setBlocked(bool blocked);

    void lockMutex();
    void unlockMutex();

private:
    // for dead zone
    void attenuate(ldVertex &v) const;
    bool isOn(float x, float y) const;
    ldVec2 getBorderPoint(const ldVertex &lastV, const ldVertex &v, bool m_isLastOn) const;
    ldDeadzoneFilter::Deadzone getDeadzone(float x, float y) const;
    void pinToBorder(ldVertex &v);

    // list of deadzones
    QList<Deadzone> m_deadzones;
    int m_selectedIndex = 0;

    /** in/out filter control */
    bool m_reverse = false;
    bool m_enabled = false;
    bool m_blocked = false;

    // filter related items
    bool m_isLastOn = false;
    ldVertex  m_lastV;

    ldVec2 m_lastBorder;
    int m_borderCount = 0;
    int MAX_BORDER_COUNT = 2;


    mutable QMutex m_mutex;
};

#endif // LDDEADZONEFILTER_H
