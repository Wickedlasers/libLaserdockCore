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

// Template class for a thread-safe queue
// Implemented with QMutex and QVector
// provides fifo operation on T[] arrays
// use constructor to set size.
// ldThreadSafeQueue<int> * pointer = new ldThreadSafeQueue<int>(256);

#ifndef LDTHREADSAFEQUEUE_H
#define LDTHREADSAFEQUEUE_H

#include <ldCore/ldCore_global.h>

template<class T> class LDCORESHARED_EXPORT ldThreadSafeQueue {
public:
    int maxSize;
    int nFull = 0;
    QVector<T> buffer;
    QMutex mutex;

    ldThreadSafeQueue<T>(int _maxSize = 1)
        : maxSize(_maxSize)
        , buffer(maxSize)
    {
    }

    ~ldThreadSafeQueue() {
    }

    int getSize() {
        QMutexLocker locker(&mutex);
        return nFull;
    }
    void deQueue(T * v, int size = 1) {
        QMutexLocker locker(&mutex);
        if (nFull < size) return;
        for (int i = 0; i < size; i++) {
            v[i] = buffer.data()[i];
        }
        nFull -= size;
        for (int i = 0; i < nFull; i++) {
            buffer.data()[i] = buffer.data()[i+size];
        }
    }
    bool enQueue(T * v, int size = 1) {
        QMutexLocker locker(&mutex);
        if (nFull + size > maxSize) return false;
        for (int i = 0; i < size; i++) {
            buffer.data()[nFull + i] = v[i];
        }
        nFull += size;
        return true;
    }
    int get(T * v, int size = 1) {
        QMutexLocker locker(&mutex);
        for (int i = 0; i < size; i++) {
            v[i] = buffer.data()[i];
        }
        return nFull;
    }
    void set(T * v, int size = 1) {
        QMutexLocker locker(&mutex);
        for (int i = 0; i < size; i++) {
            buffer.data()[i] = v[i];
        }
        nFull = size;
    }

    void clear() {
        QMutexLocker locker(&mutex);
        buffer.clear();
        nFull = 0;
    }
};

#endif // LDTHREADSAFEQUEUE_H
