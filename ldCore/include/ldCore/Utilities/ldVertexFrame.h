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

#ifndef LDVERTEXFRAME_H
#define LDVERTEXFRAME_H

#include <ldCore/ldCore_global.h>

#include <ldCore/Utilities/ldVertex.h>

/** Laser vertex frame */
class LDCORESHARED_EXPORT ldVertexFrame
{
public:
    ldVertexFrame();
    ~ldVertexFrame();

    const std::vector<ldVertex> &frame() const;
    std::vector<ldVertex> &frame();

    // std::vector wrappers
    bool empty() const;
    size_t size() const;

    void reserve(size_t size);
    void resize(size_t size);
    void clear();

    void push_back(const ldVertex &v);
    void push_back(ldVertex &&v);

    const ldVertex &operator[](size_t index) const;
    ldVertex &operator[](size_t index);

    // custom helper functions
    void resizeSmart(size_t maxFrameSize);

private:
    std::vector<ldVertex> m_frame;
};



#endif // LDVERTEXFRAME_H
