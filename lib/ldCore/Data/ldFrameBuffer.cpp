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

#include "ldFrameBuffer.h"

#include "ldCore/ldCore.h"
#include "ldCore/Filter/ldFilterBasicData.h"
#include "ldCore/Filter/ldFilter.h"
#include "ldCore/Filter/ldFilterManager.h"

#define LDFRAMEBUFFER_WARNING_FILL 300

namespace {

    inline uint16_t GetUInt16(float f)
    {
        if (f > 1.0f) f = 1.0f;
        if (f < -1.0f) f = -1.0f;
        // bad code...every time this locks up the mutex in appstate object
        //float resize_factor = (ldCore::instance()->appStateManager()->getRenderMode() == ANIMATION)?4.0f:1.0f;
        float resize_factor = 1.0f;
        return (uint16_t) (((f/resize_factor + 1.0f) * 4095.0f) / 2.0f);
    }

    inline uint8_t GetUInt8(float f){
        if (f < 0) f = 0;
        if (f > 1) f = 1;
        uint32_t color =  (f * 255);
        if(color>255) color = 255;
        return  (uint8_t) color;
    }

//    inline uint32_t GetColor(float r, float g, float b)
//    {
//        uint32_t R = (uint32_t) (r*255);
//        uint32_t G = (uint32_t) (g*255);
//        uint32_t B = (uint32_t) (b*255);

//        uint32_t color = 0;
//        color = R | G << 8 | B << 16;

//        return color;
//    }

    inline CompressedSample compress_sample(const Vertex& v){
        CompressedSample s;

        uint8_t red     = GetUInt8(v.color[0]);
        uint8_t green   = GetUInt8(v.color[1]);
        s.rg = (0x00FF & red) | ((0x00FF & green) << 8);
        s.b = (uint16_t) GetUInt8(v.color[2]);
        s.x = GetUInt16(v.position[0]);
        s.y = GetUInt16(v.position[1]);
        //qDebug() << "(r, g, b ) = (" << red << ", " << green << ", " << s.b << " )";
        return s;
    }
}
//#include <QtCore/QMutex>

ldFrameBuffer::ldFrameBuffer(QObject *parent) :
    QObject(parent)
{
}

ldFrameBuffer::~ldFrameBuffer()
{
}

void ldFrameBuffer::push(Vertex& val, bool skip_filters, bool alter_val){
//    QMutexLocker lock(&m_mutex);

    if (m_fill >= FRAMEBUFFER_CAPACITY) return;

    if(!skip_filters){
        // filter if necessary
        Vertex tval = val;
        Vertex simVal, dataVal;

        // apply data filter
        ldFilterManager *fm = ldCore::instance()->filterManager();
        fm->m_basicGlobalFilter.process(tval);
        // apply global filter to simulator output
        ldFilter *globalFilter = fm->m_globalFilter;
        if (globalFilter)
            globalFilter->process(tval);

        // store similator value
        simVal = tval;

        // apply data filter to data output
        // give filter proper settings
        fm->m_dataFilter.frameModes = m_frameModes;
        fm->m_dataFilter.process(tval);

        // store data value
        dataVal = tval;

        // compress and add to buffer
        m_buffer[m_fill] = simVal;
        m_compressed_buffer[m_fill] = compress_sample(dataVal);
        if (alter_val) val = simVal; // save this for screenshot feature
    } else {

        m_buffer[m_fill] = val;

        m_compressed_buffer[m_fill] = compress_sample(val);
    }
    m_fill++;

}

unsigned int ldFrameBuffer::get(Vertex * pbuffer, CompressedSample &pcbuffer, unsigned int size)
{
//    QMutexLocker lock(&m_mutex);

    // check for available
    int available = m_fill - m_exhuasted_index;
    if(available <= 0) {
        return 0;
    }

    // what buffer should be sent
    unsigned int actual = std::min((unsigned int) available, size);

    // pbuffer is optional
    if(pbuffer) {
        memcpy(pbuffer, (m_buffer + m_exhuasted_index), actual*sizeof(Vertex));
    }
    memcpy(&pcbuffer, (m_compressed_buffer + m_exhuasted_index), actual*sizeof(CompressedSample));

    m_exhuasted_index += actual;

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
    //if(m_fill < LDFRAMEBUFFER_WARNING_FILL) qDebug() << "Framebuffer fill below warning level.";
    m_isFilled = true;
}

void ldFrameBuffer:: setFrameModes(int flags) {
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
