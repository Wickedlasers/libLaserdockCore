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

#ifndef LDBASICDATASTRUCTURES_H
#define LDBASICDATASTRUCTURES_H

#include <stdlib.h>
#include <stdint.h>

#include <QtCore/QMutexLocker>

#include <ldCore/ldCore_global.h>

/**
 * Common structs and buffers
 */


/** Laser vertex. Used for easier processing */
struct LDCORESHARED_EXPORT Vertex
{
    /** x,y,z */
    float position[3];
    /** r,g,b,opacity [0..1] */
    float color[4];

    void clear();

    bool isValid() const;
    bool isBlank() const;
};

/** Samples that are used by laser hardware */
struct LDCORESHARED_EXPORT CompressedSample
{
    static uint16_t flipCoord(uint16_t value);

    CompressedSample();
    CompressedSample(const Vertex &v);

    uint16_t rg = 0;
    uint16_t b = 0;
    uint16_t x = 0;
    uint16_t y = 0;

private:
    uint16_t GetUInt16(float f) const;
    uint8_t GetUInt8(float f) const;
};

inline uint16_t CompressedSample::flipCoord(uint16_t value)
{
    return 4095 - value;
}

inline uint16_t CompressedSample::GetUInt16(float f) const
{
    if (f > 1.0f) f = 1.0f;
    if (f < -1.0f) f = -1.0f;
    return (uint16_t) ((f + 1.0f) * 2047.5f);
}

inline uint8_t CompressedSample::GetUInt8(float f) const
{
    if (f < 0) f = 0;
    if (f > 1) f = 1;
    uint32_t color =  (f * 255);
    if(color>255) color = 255;
    return  (uint8_t) color;
}


/** Internal buffer */
template<class T>
class LDCORESHARED_EXPORT ldBuffer
{
public:
    ldBuffer()
        : m_size(0)
        , m_data(NULL)
    {}
    ldBuffer(unsigned size)
        : m_size(size)
        , m_data(NULL)
    {
        if (m_size > 0)
        {
            m_data = new T[size];
        }
    }
    ldBuffer(const ldBuffer& other)
        : m_size(other.m_size)
        , m_data(NULL)
    {
        if (other.m_data != NULL)
        {
            m_data = new T[m_size];
            for (unsigned i = 0; i < m_size; ++i)
            {
                m_data[i] = other.m_data[i];
            }
        }
        else
        {
            m_data = NULL;
        }
    }
    virtual ~ldBuffer()
    {
        if (m_data != NULL)
        {
            delete[] m_data;
        }
    }

    inline unsigned GetSize() const { return m_size; }

    inline const T* GetData() const { return m_data; }
    inline T* GetData() { return m_data; }

    void Resize(unsigned size, bool copy = false)
    {
        if (m_size == size) return;

        if (size > 0)
        {
            T* new_data = new T[size];

            if (m_data != NULL)
            {
                if (copy)
                {
                    unsigned s = (size < m_size ? size : m_size);
                    for (unsigned i = 0; i < s; ++i)
                    {
                        new_data[i] = m_data[i];
                    }
                }
                delete[] m_data;
            }

            m_data = new_data;
        }
        else
        {
            delete[] m_data;
            m_data = NULL;
        }

        m_size = size;
    }

    void operator = (const ldBuffer& other)
    {
        if (m_data != NULL)
        {
            delete[] m_data;
            m_data = NULL;
        }

        m_size = other.m_size;

        if (m_size > 0)
        {
            m_data = new T[m_size];
            for (unsigned i = 0; i < m_size; ++i)
            {
                m_data[i] = other.m_data[i];
            }
        }
    }

    inline const T& operator [] (unsigned ind) const { return m_data[ind]; }
    inline T& operator [] (unsigned ind) { return m_data[ind]; }

private:
    unsigned    m_size;
    T*          m_data;
};

template<class T>
class LDCORESHARED_EXPORT ldAudioBuffer
{
public:
    ldAudioBuffer(unsigned size)
        : m_size(size)
        , m_left(size)
        , m_right(size)
    {
    }
    ~ldAudioBuffer()
    {
    }

    inline unsigned GetSize() const { return m_size; }

    inline const T* GetLeft() const { return m_left.GetData(); }
    inline T* GetLeft() { return m_left.GetData(); }

    inline const T& GetLeft(unsigned ind) const { return m_left[ind]; }

    inline const T* GetRight() const { return m_right.GetData(); }
    inline T* GetRight() { return m_right.GetData(); }

    inline const T& GetRight(unsigned ind) const { return m_right[ind]; }

private:
    unsigned    m_size;     // Size of audio data in samples
    ldBuffer<T> m_left;     // Left channel data
    ldBuffer<T> m_right;    // Right channel data
};

/*
 circular buffer does not have a tail.
 data is continously fed into the head and push head forward.
*/
template<class T>
class LDCORESHARED_EXPORT ldCircularBuffer: private ldBuffer<T>
{
public:
    ldCircularBuffer(unsigned size)
        : ldBuffer<T>(size)
        , m_head(0)
    {}
    virtual ~ldCircularBuffer() {}

    /*--------------------------------------------------//
     Push data to the buffer
     [parameters]
     data       - pointer to the data array
     size       - size of data array (in items)
     //--------------------------------------------------*/
    void Push(const T* data, unsigned size)
    {
        T* ptr = ldBuffer<T>::GetData() + m_head;
        unsigned remained = ldBuffer<T>::GetSize() - m_head;

        if (remained >= size)
        {
            memcpy(ptr, data, size * sizeof(T));

            if (remained > size)
            {
                m_head += size;
            }
            else
            {
                m_head = 0;
            }
        }
        else
        {
            memcpy(ptr, data, remained * sizeof(T));

            ptr = ldBuffer<T>::GetData();
            data += remained;
            memcpy(ptr, data, (size - remained) * sizeof(T));

            m_head = size - remained;
        }
    }
    /*--------------------------------------------------//
     Push value to the buffer
     [parameters]
     val        - value
     //--------------------------------------------------*/
    void Push(const T& val)
    {
        ldBuffer<T>::GetData()[m_head] = val;
        ++m_head;
        if (m_head == ldBuffer<T>::GetSize())
        {
            m_head = 0;
        }
    }
    /*--------------------------------------------------//
     Get data from the buffer
     [parameters]
     data       - pointer to the data array
     size       - size of data array (in items)
     //--------------------------------------------------*/
    void Get(T* data, unsigned size)
    {
        if (size <= m_head)
        {
            T* ptr = ldBuffer<T>::GetData() + m_head - size;
            memcpy(data, ptr, size * sizeof(T));
        }
        else
        {
            T* ptr = ldBuffer<T>::GetData() + ldBuffer<T>::GetSize() - (size - m_head);
            memcpy(data, ptr, (size - m_head) * sizeof(T));

            ptr = ldBuffer<T>::GetData();
            data += size - m_head;
            memcpy(data, ptr, m_head * sizeof(T));
        }
    }


    void Reset()
    {
        memset(ldBuffer<T>::GetData(), 0, ldBuffer<T>::GetSize() * sizeof(T));
    }

private:
    unsigned    m_head;
};

///TODO  create a ringbuffer with tail and head similar to what is in laserdock hardware

template<class T>
class ldRingBuffer: private ldBuffer<T>
{
public:
    ldRingBuffer(unsigned size)
    : ldBuffer<T>(size)
    , m_head(0)
    , m_tail(0)
    , m_mutex(new QMutex)
    {

    }

    virtual ~ldRingBuffer() {
        delete m_mutex;
    }

    unsigned FillCount(){
        QMutexLocker locker(m_mutex);
        return __fillcount();
    }

    unsigned AvailableCount(){
        QMutexLocker locker(m_mutex);
        return __availablecount();
    }

    /*--------------------------------------------------//
     Push data to the buffer
     [parameters]
     data       - pointer to the data array
     size       - size of data array (in items)
     //--------------------------------------------------*/
    void Push(const T* data, unsigned size)
    {
        QMutexLocker locker(m_mutex);

        /// not enough space, we have to erase some existing samples to make room.
        if(__availablecount() < size){
            m_tail = (m_head+size+1) % ldBuffer<T>::GetSize();
        }

        T* ptr = ldBuffer<T>::GetData() + m_head;
        unsigned remained = ldBuffer<T>::GetSize() - m_head;

        if (remained >= size)
        {
            memcpy(ptr, data, size * sizeof(T));
        }
        else
        {
            memcpy(ptr, data, remained * sizeof(T));
            ptr = ldBuffer<T>::GetData();
            data += remained;
            memcpy(ptr, data, (size - remained) * sizeof(T));
        }

        m_head = (m_head + size) % ldBuffer<T>::GetSize();
    }
    /*--------------------------------------------------//
     Push value to the buffer
     [parameters]
     val        - value
     //--------------------------------------------------*/
    void Push(const T& val)
    {
        QMutexLocker locker(m_mutex);
        if(m_tail == (m_head + 1) % ldBuffer<T>::GetSize())
            m_tail = (m_tail + 1) % ldBuffer<T>::GetSize();

        ldBuffer<T>::GetData()[m_head] = val;
        m_head = (m_head + 1) % ldBuffer<T>::GetSize();
    }
    /*--------------------------------------------------//
     Get data from the buffer
     [parameters]
     data       - pointer to the data array
     size       - size of data array (in items)
     @return    - size of sample points actually fetched
     //--------------------------------------------------*/

    unsigned int Get(T* data, unsigned size = 1, bool update_tail = true)
    {
        QMutexLocker locker(m_mutex);

        if(__fillcount() == 0 || size == 0) return 0;

        unsigned actual_size = size;

        if(__fillcount() < size){
            actual_size = __fillcount();
        }

        T* ptr = ldBuffer<T>::GetData() + m_tail;

        int remained = actual_size - (ldBuffer<T>::GetSize() - m_tail);

        if (remained <= 0)
        {
            memcpy(data, ptr, actual_size * sizeof(T));
        }
        else
        {
            memcpy(data, ptr, (actual_size - remained) * sizeof(T));
            ptr = ldBuffer<T>::GetData();
            data += remained;
            memcpy(data, ptr,  remained * sizeof(T));
        }

        if(update_tail)
            m_tail = (m_tail + actual_size) % ldBuffer<T>::GetSize();
        return actual_size;

    }

    void Reset()
    {
        QMutexLocker locker(m_mutex);
        m_tail = m_head = 0;
        memset(ldBuffer<T>::GetData(), 0, ldBuffer<T>::GetSize() * sizeof(T));
    }

private:

    unsigned __fillcount(){
        if(m_head >= m_tail){
            return m_head - m_tail;
        } else {
            return ldBuffer<T>::GetSize() - (m_tail - m_head);
        }
    }

    unsigned __availablecount(){
        return ldBuffer<T>::GetSize() - __fillcount();
    }

    unsigned    m_head;         ///first slot that is avaiable for sample storage.
    unsigned    m_tail;         ///first slot that is avaiable for sample playback.

    QMutex *m_mutex;
};


typedef ldCircularBuffer<Vertex> ldVertexCircularBuffer;
typedef ldRingBuffer<Vertex> ldVertexRingBuffer;
typedef ldRingBuffer<CompressedSample> ldCompressedSampleRingBuffer;


// class for color curve
class LDCORESHARED_EXPORT ldColorCurve {
public:
    float thold = 0;
    float gain = 1;
    float dflect = 0;

    float get(float f) {
        // old
        /* float a, b, z;
         if      (f <= 0.25) {a =  0; b = x1; z = (f-0.00)*4;}
         else if (f <= 0.50) {a = x1; b = x2; z = (f-0.25)*4;}
         else if (f <= 0.75) {a = x2; b = x3; z = (f-0.50)*4;}
         else                {a = x3; b =  1; z = (f-0.75)*4;}
         return (1-z)*a + (z)*b;*/

        float midx = (1-dflect)/2;
        float midy = ((1+dflect)/2) * (gain-thold) + thold;

        if (f < midx) {
            float slope = (midy-thold)/(midx-0);
            return  slope*(f-0) + thold;
        } else {
            float slope = (gain-midy)/(1-midx);
            return  slope*(f-midx) + midy;
        }
    }
};

#endif // LDBASICDATASTRUCTURES_H
