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

#ifndef LDFILTERMANAGER_H
#define LDFILTERMANAGER_H

#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"

#include "ldFilterBasicGlobal.h"
#include "ldFilterBasicData.h"

class ldFilter;

class LDCORESHARED_EXPORT ldFilterManager : public QObject
{
    Q_OBJECT

public:
    explicit ldFilterManager(QObject *parent = 0);    

    void setGlobalFilter(ldFilter *globalFilter);
    void setFrameModes(int frameModes);

    void process(Vertex &tval, Vertex &simVal);

    ldColorCurveFilter *baseColorCurveFilter() const;
    ldColorCurveFilter *colorCurveFilter() const;
    ldDeadzoneFilter *deadzoneFilter() const;
    ldHueFilter *hueFilter() const;
    ldHueShiftFilter *hueShiftFilter() const;
    ldFlipFilter *flipFilter() const;
    ldScaleFilter *scaleFilter() const;
    ldShiftFilter *shiftFilter() const;
    ldSoundLevelFilter *soundLevelFilter() const;
    ldRotateFilter *rotateFilter() const;
    ldTracerFilter *tracerFilter() const;

    void setBrightness(float brightness);
    void setKeystoneX(float keystoneX);
    void setKeystoneY(float keystoneY);
    void setOffset(int offset);
    void setTtl(bool isTtl);

private:
    ldFilterBasicGlobal m_basicGlobalFilter;
    ldFilterBasicData m_dataFilter;
    ldFilter* m_globalFilter = nullptr;

    std::unique_ptr<ldFlipFilter> m_flipFilter;
    std::unique_ptr<ldRotateFilter> m_rotateFilter;
};

#endif // LDFILTERMANAGER_H
