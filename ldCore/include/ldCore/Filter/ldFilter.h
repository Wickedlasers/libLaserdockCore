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

#ifndef LDFILTER_H
#define LDFILTER_H

#include <QQmlHelpers>

#include <ldCore/Shape/ldShader.h>
#include <ldCore/Utilities/ldVertex.h>

class ldFilterProcessor;

/** Abstract filter interface. Filters are applied to Vertex data directly */
class LDCORESHARED_EXPORT ldFilter : public ldShader
{
    Q_OBJECT
    LD_WRITABLE_MIN_MAX_PROPERTY(float, intensity)
public:
    ~ldFilter();

    /** Apply this filter to Vertex data */
    void processFilter(ldVertex &input);
    /** Apply this filter to Vertex data */
    void processFilterWithoutProcessor(ldVertex &input);

    /** Set optional advanced filter processor  */
    void setProcessorFilter(ldFilterProcessor *processorFilter);

    /** In case if filter need to be restarted on different frame you can clean cache here */
    virtual void resetFilter() {}

    /** Optional filter name */
    virtual QString name() { return QString(); }

    /** Optional flag. Can be set by developer explicitly if filter is reactive to music analyzer */
    virtual bool isMusicAware() const { return false; }

    virtual bool isShowProperties() const { return true; }
protected:
    ldFilter();

    /** Filters implement this function to filter points.
        Input is the source vertex, processing is done in-place with a ref param */
    virtual void process(ldVertex &v) = 0;

private:
    // shader function adapter
    virtual void ShaderFunc(float *x, float *y, uint32_t *color) override;

    ldFilterProcessor *m_processorFilter = nullptr;

    // make it friend to call to ::process function directly
    friend class ldFilterProcessor;
};

#endif // LDFILTER_H

