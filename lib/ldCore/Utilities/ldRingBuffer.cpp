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

#include "ldRingBuffer.h"
#include <QtCore/QDebug>


typedef struct circular_buffer {
    void *buffer;     // data buffer
    void *buffer_end; // end of data buffer
    size_t capacity;  // maximum number of items in the buffer
    size_t count;     // number of items in the buffer
    size_t sz;        // size of each item in the buffer
    void *head;       // pointer to head
    void *tail;       // pointer to tail
} circular_buffer;


ldRingBuffer::ldRingBuffer(size_t queueCapacity, size_t frameCount, QObject *parent) :
    QObject(parent),
    m_queueCapacity(queueCapacity),
    m_frameCount(frameCount)
{
    initCB();
}

ldRingBuffer::~ldRingBuffer(){
    delete m_cb;
}

void ldRingBuffer::initCB(){
    /* Initialise everything before
     * using it.
     */
    m_cb = new circular_buffer;
    m_cb->buffer = malloc(m_queueCapacity * (m_frameCount * sizeof(float)));
    if(m_cb->buffer == NULL){
        qDebug() << "RingBuffer:: circular_buffer == NULL";
    }
    m_cb->buffer_end = (char *) m_cb->buffer + m_queueCapacity * (m_frameCount * sizeof(float));
    m_cb->capacity = m_queueCapacity;
    m_cb->count = 0;
    m_cb->sz = m_frameCount * sizeof(float);
    m_cb->head = m_cb->buffer;
    m_cb->tail = m_cb->buffer;
    isFull = false;
}

void ldRingBuffer::writeTail(void *item){
    if(m_cb->buffer_end < m_cb->tail){
        qDebug() << "Writing past end.";
    }
    /*
     * Copy buffer to the tail;
     */
    memcpy(m_cb->tail, item, m_cb->sz);
    /*
     * move the tail pointer along by sz (frame size);
     */
    m_cb->tail = (char *)m_cb->tail + m_cb->sz;
    if(m_cb->tail == m_cb->buffer_end){
        /* loop writes */
        m_cb->tail = m_cb->buffer;
    }
    m_cb->count++;
    /*
     * Has the buffer been filled yet?
     * Lets send a wait flag if it has.
     */
    if(m_cb->count >= m_cb->capacity){
        isFull = true;
    }
}

void ldRingBuffer::readHead(void *item){
    if(m_cb->count == 0){
        /* No Items!
         * This happens when either the io
         * has come to a hault, the buffer
         * isn't long enough, the framesPerBuffer
         * is too low/short.
         * You should never get there.
         */
        qDebug() << "RingBuffer::readHead: No Items.";
    }
    memcpy(item, m_cb->head, m_cb->sz);
    m_cb->head = (char*)m_cb->head + m_cb->sz;
    if(m_cb->head == m_cb->buffer_end){
        m_cb->head = m_cb->buffer;
    }
    m_cb->count--;
    if(m_cb->count < m_cb->capacity){
        isFull = false;
    }
}

void ldRingBuffer::purge(){
    /* Reset the count to 0
     * which means we can overwrite
     * each segment of the buffer.
     * Set isFull to false which
     * starts the buffer reading again.
     */
    qDebug() << "RingBuffer: purging.";
    m_cb->count = 0;
    m_cb->head = m_cb->buffer;
    m_cb->tail = m_cb->buffer;
    isFull = false;
}
