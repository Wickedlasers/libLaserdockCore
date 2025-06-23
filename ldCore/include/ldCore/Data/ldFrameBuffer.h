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

#ifndef LDFRAMEBUFFER_H
#define LDFRAMEBUFFER_H

#include <QtCore/QMutex>
#include <QtCore/QObject>

#include <ldCore/ldCore_global.h>
#include <ldCore/Utilities/ldVertexFrame.h>

class ldHardwareBatch;
class ldVertexFrame;

class LDCORESHARED_EXPORT ldFrameBuffer : public QObject
{
    Q_OBJECT

public:
    static const int FRAMEBUFFER_CAPACITY = 4000;

    explicit ldFrameBuffer(ldHardwareBatch *hardwareBatch, QObject *parent = 0);
    ~ldFrameBuffer();

    /** Add simulator value, the same value goes to output with minimal possible safety filters */
    void push(const ldVertex &val);

    /** Add simulator/output vectors */
    void pushFrame(ldVertexFrame &frame);

    /** Read data for sim/output */
    uint get(ldVertex * pbuffer, uint size);

    /** Clear current buffer, request for more data */
    void reset();

    /** Finish adding data */
    void commit();

    /** Special case when laser need more data preserving the existing data */
    void requestMore();

    /** Get available size of data to read */
    uint getAvailable() const;

    /** Is buffer ready to read */
    bool isFilled() const;

    uint getExhuastedIndex() const;
signals:
    void isCleaned();

private:
//    QMutex m_mutex;
    ldHardwareBatch *m_hardwareBatch;

    uint m_exhuasted_index = 0;
    uint m_fill = 0;
    bool m_isFilled = false;

    std::vector<ldVertex> m_buffer;
};

#endif // LDFRAMEBUFFER_H
