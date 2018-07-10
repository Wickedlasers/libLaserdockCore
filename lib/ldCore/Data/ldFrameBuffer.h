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
#include <QtCore/QMutexLocker>
#include <QtCore/QObject>

#include "ldCore/ldCore_global.h"
#include "ldCore/Utilities/ldBasicDataStructures.h"

#define FRAMEBUFFER_CAPACITY 4000

class ldFilter;

class LDCORESHARED_EXPORT ldFrameBuffer : public QObject
{
    Q_OBJECT

public:
    explicit ldFrameBuffer(QObject *parent = 0);
    ~ldFrameBuffer();

    void push(Vertex& val, bool skip_filters = false, bool alter_val = false);
    unsigned int get(Vertex * pbuffer, CompressedSample &pcbuffer, unsigned int size);

    void setFrameModes(int flags);

    void reset();
    void commit();

    qint32 getAvailable() const;
    bool isFilled() const;

signals:
    void isCleaned();

private:
    QMutex m_mutex;

    qint32 m_exhuasted_index = 0;
    qint32 m_fill = 0;
    bool m_isFilled = false;

    Vertex m_buffer[FRAMEBUFFER_CAPACITY] = {};
    CompressedSample m_compressed_buffer[FRAMEBUFFER_CAPACITY] = {};

    int m_frameModes = 0;
};

#endif // LDFRAMEBUFFER_H
