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

#ifndef LDBUFFER_H
#define LDBUFFER_H

#include <ldCore/ldCore_global.h>

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
        if(this == &other)
            return;

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

#endif // LDBUFFER_H
