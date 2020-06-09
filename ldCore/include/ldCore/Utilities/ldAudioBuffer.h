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

#ifndef LDAUDIOBUFFER_H
#define LDAUDIOBUFFER_H

#include <ldCore/ldCore_global.h>
#include <ldCore/Utilities/ldBuffer.h>

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

#endif // LDAUDIOBUFFER_H
