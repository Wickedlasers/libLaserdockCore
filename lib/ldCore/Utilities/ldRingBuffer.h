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

#ifndef LDRINGBUFFER_H
#define LDRINGBUFFER_H

#include <QtCore/QObject>

struct circular_buffer;

class ldRingBuffer : public QObject
{
public:
    ldRingBuffer(size_t queueCapacity, size_t frameCount, QObject *parent=0);
    ~ldRingBuffer();
    void initCB();
    void readHead(void *item);
    void writeTail(void *item);
    void purge();

    bool isFull;
    size_t m_queueCapacity;
    size_t m_frameCount;
    circular_buffer *m_cb;

};

#endif // LDRINGBUFFER_H
