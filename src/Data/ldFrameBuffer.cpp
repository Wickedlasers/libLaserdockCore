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

#include "ldCore/Data/ldFrameBuffer.h"

#include "ldCore/ldCore.h"
#include "ldCore/Filter/ldFilterBasicData.h"
#include "ldCore/Filter/ldFilter.h"
#include "ldCore/Filter/ldFilterManager.h"

#define LDFRAMEBUFFER_WARNING_FILL 300

ldFrameBuffer::ldFrameBuffer(QObject *parent) :
    QObject(parent)
{
}

ldFrameBuffer::~ldFrameBuffer()
{
}

void ldFrameBuffer::push(Vertex& val, bool skip_filters, bool alter_val)
{
//    QMutexLocker lock(&m_mutex);

    if (m_fill >= FRAMEBUFFER_CAPACITY) return;

    if(!skip_filters){
        // filter if necessary
        Vertex tval = val;
        Vertex simVal;

        // apply data filter
        ldFilterManager *fm = ldCore::instance()->filterManager();

        fm->setFrameModes(m_frameModes);
        fm->process(tval, simVal);

        // compress and add to buffer
        m_buffer[m_fill] = simVal;
        m_compressed_buffer[m_fill] = CompressedSample(tval);
        if (alter_val) val = simVal; // save this for screenshot feature
    } else {
        m_buffer[m_fill] = val;
        m_compressed_buffer[m_fill] = CompressedSample(val);
    }
    m_fill++;
}

unsigned int ldFrameBuffer::get(Vertex * pbuffer, CompressedSample &pcbuffer, unsigned int size)
{
//    QMutexLocker lock(&m_mutex);

    // check for available
    unsigned int actual = 0;
    int available = m_fill - m_exhuasted_index;
    if(available > 0) {
        // what buffer should be sent
        actual = std::min((unsigned int) available, size);

        // pbuffer is optional
        if(pbuffer) memcpy(pbuffer, (m_buffer + m_exhuasted_index), actual*sizeof(Vertex));
        memcpy(&pcbuffer, (m_compressed_buffer + m_exhuasted_index), actual*sizeof(CompressedSample));

        m_exhuasted_index += actual;
    }

    return actual;
}

void ldFrameBuffer::reset()
{
    m_exhuasted_index = 0;
    m_fill = 0;
    m_isFilled = false;

    emit isCleaned();
}

void ldFrameBuffer::commit()
{
    m_isFilled = true;
}

void ldFrameBuffer::setFrameModes(int flags) {
    m_frameModes = flags;
}

qint32 ldFrameBuffer::getAvailable() const
{
    qint32 localBuffer = 0;
    if (m_isFilled) {
        localBuffer = m_fill - m_exhuasted_index;
    }

    return localBuffer;
}

bool ldFrameBuffer::isFilled() const{
    return m_isFilled;
}
