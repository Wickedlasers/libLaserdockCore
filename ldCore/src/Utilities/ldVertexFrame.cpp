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

#include "ldCore/Utilities/ldVertexFrame.h"


ldVertexFrame::ldVertexFrame()
{
}

ldVertexFrame::~ldVertexFrame()
{
}

const std::vector<ldVertex> &ldVertexFrame::frame() const
{
    return m_frame;
}

std::vector<ldVertex> &ldVertexFrame::frame()
{
    return m_frame;
}

bool ldVertexFrame::empty() const
{
    return m_frame.empty();
}

size_t ldVertexFrame::size() const
{
    return  m_frame.size();
}

void ldVertexFrame::reserve(size_t size)
{
    m_frame.reserve(size);
}

void ldVertexFrame::resize(size_t size)
{
    m_frame.resize(size);
}

void ldVertexFrame::clear()
{
    m_frame.clear();
}

void ldVertexFrame::push_back(const ldVertex &v)
{
    m_frame.push_back(v);
}

void ldVertexFrame::push_back(ldVertex &&v)
{
    m_frame.push_back(v);
}

void ldVertexFrame::resizeSmart(size_t maxFrameSize)
{
    if(maxFrameSize > m_frame.size() && m_frame.size() > 0)
        m_frame.insert(m_frame.end(), maxFrameSize - m_frame.size(), m_frame[m_frame.size() - 1]);
}

const ldVertex &ldVertexFrame::operator[](size_t index) const
{
    return m_frame[index];
}

ldVertex &ldVertexFrame::operator[](size_t index)
{
    return m_frame[index];
}
