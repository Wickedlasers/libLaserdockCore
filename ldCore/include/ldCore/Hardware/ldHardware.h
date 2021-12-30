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

#ifndef LDHARDWARE_H
#define LDHARDWARE_H

#include <QtCore/QObject>
#include <QtCore/QDateTime>

#include <ldCore/ldCore_global.h>
#include <ldCore/Utilities/ldCompressedSample.h>
#include <ldCore/Utilities/ldVertexFrame.h>

class ldHardwareFilter;

class LDCORESHARED_EXPORT ldHardware : public QObject
{
    Q_OBJECT

public:
    static uint REMOTE_MAX_BUFFER;

    enum class Status { UNKNOWN, INITIALIZED };
    
    ~ldHardware();

    virtual QString id() const = 0;
    virtual QString hwType() const = 0;
    virtual QString address() const = 0;
    Status status();

    bool isActive() const;
    void setActive(bool active);

    void setFrame(uint index, size_t count);
    void setSample(uint index, const ldVertex &sample);

    void setFilter(ldHardwareFilter *filter);
    ldHardwareFilter *filter() const;

signals:
    void statusChanged(ldHardware::Status status);

protected:
    explicit ldHardware(QObject *parent = nullptr);

    void setStatus(Status status);

    bool m_isActive = false;

    std::vector<ldCompressedSample> m_compressed_buffer;

private:
    ldHardwareFilter *m_filter = nullptr;
    Status m_status = Status::UNKNOWN;

};

Q_DECLARE_METATYPE(ldHardware *)

#endif // LDHARDWARE_H
