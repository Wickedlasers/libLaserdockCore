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

#include "ldCore/Hardware/ldHardware.h"
#include "ldCore/Hardware/ldHardwareInfo.h"
#include <QtDebug>
#include <ldCore/ldCore.h>
#include <ldCore/Data/ldFrameBuffer.h>
#include <ldCore/Filter/ldDeadzoneFilter.h>
#include <ldCore/Filter/ldFilterManager.h>

#include <ldCore/Filter/ldHardwareFilter.h>

uint ldHardware::REMOTE_MAX_BUFFER = 768;

ldHardware::~ldHardware()
{
}

ldHardware::Status ldHardware::status() const
{
    return m_status;
}

bool ldHardware::isEnabled() const
{
    return m_enabled;
}

void ldHardware::setEnabled(bool en)
{
    if (en!=m_enabled) {
        m_enabled = en;
        emit enabledChanged(en);
    }
}

// device is only active if it is flagged as such, and it has not been disabled
bool ldHardware::isActive() const
{
    return m_isActive && m_enabled;
}

void ldHardware::setActive(bool active)
{
    m_isActive = active;
}

void ldHardware::setFrame(uint index, size_t count)
{
    Q_ASSERT(m_filter != nullptr);

    for(uint i = 0; i < count; i++) {
        Q_ASSERT(index + i < m_compressed_buffer.size());
        m_compressed_buffer[index + i] = ldCompressedSample(m_filter->lastFrame()[i]);
    }
}

void ldHardware::setSample(uint index, const ldVertex &sample)
{
//    if(!m_isActive)
//        return;

    Q_ASSERT(index < m_compressed_buffer.size());

    m_compressed_buffer[index] = ldCompressedSample(sample);
}

void ldHardware::setFilter(ldHardwareFilter *filter)
{
    m_filter = filter;
}

ldHardwareFilter *ldHardware::filter() const
{
    return m_filter;
}

void ldHardware::setBatch(ldHardwareBatch *batch)
{
    if(m_batch == batch)
        return;

    m_batch = batch;

    emit batchChanged(batch);
}

ldHardwareBatch *ldHardware::batch() const
{
    return m_batch;
}

ldHardwareInfo* ldHardware::info() const
{
    return m_info;
}

bool ldHardware::isActiveAndInitialized() const
{
    return isActive() && status() == ldHardware::Status::INITIALIZED;
}

ldHardware::ldHardware(QObject *parent)
    : QObject(parent)
    , m_info(new ldHardwareInfo())
{
    m_compressed_buffer.resize(ldFrameBuffer::FRAMEBUFFER_CAPACITY);
}

void ldHardware::setStatus(ldHardware::Status status)
{
    if(m_status == status) {
        return;
    }

    m_status = status;

    emit statusChanged(status);
}

