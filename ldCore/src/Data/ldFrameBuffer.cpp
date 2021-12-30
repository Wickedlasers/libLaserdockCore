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

#include <QtCore/QtDebug>

#include <ldCore/ldCore.h>
#include <ldCore/Filter/ldDeadzoneFilter.h>
#include <ldCore/Filter/ldFilterManager.h>
#include <ldCore/Filter/ldHardwareFilter.h>
#include <ldCore/Hardware/ldHardware.h>
#include <ldCore/Hardware/ldHardwareManager.h>
#include <ldCore/Utilities/ldVertexFrame.h>

ldFrameBuffer::ldFrameBuffer(QObject *parent)
    : QObject(parent)
{
    m_buffer.resize(FRAMEBUFFER_CAPACITY);
}

ldFrameBuffer::~ldFrameBuffer()
{
}

void ldFrameBuffer::push(const ldVertex& val)
{
    //    QMutexLocker lock(&m_mutex);

    if (m_fill >= FRAMEBUFFER_CAPACITY) return;

    m_buffer[m_fill] = val;

    for(ldHardware *hardware : ldCore::instance()->hardwareManager()->devices()) {
        ldHardwareFilter *hwFilter = hardware->filter();
        if (hwFilter) {
            ldVertex dataVal = val;
            hwFilter->processVertex(dataVal);
            hardware->setSample(m_fill, dataVal);
        }
    }

    m_fill++;
}

void ldFrameBuffer::pushFrame(ldVertexFrame &frame)
{
    //    QMutexLocker lock(&m_mutex);
    if (m_fill >= FRAMEBUFFER_CAPACITY) return;

    // process all frames
    // frame size could be different on each device so we should make all buffers the same after processing
    size_t maxFrameSize = 0;

    for(ldHardware *hardware : ldCore::instance()->hardwareManager()->devices()) {
        ldHardwareFilter *hwFilter = hardware->filter();
        if (hwFilter) {
            hwFilter->processFrame(frame);
            maxFrameSize = std::max(maxFrameSize, hwFilter->lastFrame().size());
        }
    }
    // resize all buffers to the biggest one
    for(ldHardware *hardware : ldCore::instance()->hardwareManager()->devices()) {
        ldHardwareFilter *hwFilter = hardware->filter();
        if (hwFilter) hwFilter->lastFrame().resizeSmart(maxFrameSize);
    }
    frame.resizeSmart(maxFrameSize);

    // no devices debug frame
//    if(ldCore::instance()->hardwareManager()->devices().empty()) {
//        ldHardwareFilter *stubFilter = ldCore::instance()->filterManager()->hardwareFilter();
//        stubFilter->processFrame(frame);
//        frame = stubFilter->lastFrame();
//    }

    // check that we fit in frame buffer
    size_t sizeToPush = m_fill + frame.size() >= FRAMEBUFFER_CAPACITY && !frame.empty()
                            ? FRAMEBUFFER_CAPACITY - m_fill
                            : frame.size();

    if(!sizeToPush)
        return;

    // memcpy for faster processing
    memcpy(&m_buffer[m_fill], &(frame[0]), sizeToPush * sizeof (ldVertex));

    for(ldHardware *hardware : ldCore::instance()->hardwareManager()->devices())
        hardware->setFrame(m_fill, sizeToPush);


    m_fill += sizeToPush;
}

uint ldFrameBuffer::get(ldVertex * pbuffer, uint size)
{
//    QMutexLocker lock(&m_mutex);

    // check for available
    unsigned int actual = 0;
    if(m_fill > m_exhuasted_index) {
        uint available = m_fill - m_exhuasted_index;
        // what buffer should be sent
        actual = std::min(available, size);

        // pbuffer is optional
        if(pbuffer) memcpy(pbuffer, &m_buffer[m_exhuasted_index], actual*sizeof(ldVertex));

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

void ldFrameBuffer::requestMore()
{
    // temporary buffers
    std::vector<ldVertex> buffer;
    std::vector<ldCompressedSample> compressed_buffer;

    // store available data
    uint available = getAvailable();
    if(available > 0) {
        buffer.insert(buffer.end(),
                      m_buffer.begin() + m_exhuasted_index,
                      m_buffer.begin() + m_fill);

        // TODO: implement ldHardware logic for network worker
//        compressed_buffer.insert(compressed_buffer.end(),
//                                 m_compressed_buffer.begin() + m_exhuasted_index,
//                                 m_compressed_buffer.begin() + m_fill);
    }

    // clean everything
    reset();

    // insert old data in the begin
    m_buffer.insert(m_buffer.begin(), buffer.begin(), buffer.end());
//    m_compressed_buffer.insert(m_compressed_buffer.begin(), compressed_buffer.begin(), compressed_buffer.end());
    m_fill += available;
}

uint ldFrameBuffer::getAvailable() const
{
    uint localBuffer = 0;
    if (m_isFilled
        && m_fill > m_exhuasted_index) {
        localBuffer = m_fill - m_exhuasted_index;
    }

    return localBuffer;
}

bool ldFrameBuffer::isFilled() const{
    return m_isFilled;
}

uint ldFrameBuffer::getExhuastedIndex() const
{
    return m_exhuasted_index;
}
