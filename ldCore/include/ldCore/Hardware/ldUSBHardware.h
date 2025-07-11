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

#ifndef LDUSBHARDWARE_H
#define LDUSBHARDWARE_H

#include "ldHardware.h"

class LaserdockDevice;
struct LaserdockSample;
class ldUSBHardwarePrivate;

class LDCORESHARED_EXPORT ldUSBHardware : public ldHardware {
    
    Q_OBJECT
    
public:
    struct device_params {
        uint32_t version_major_number = 0;
        uint32_t version_minor_number = 0;
        uint32_t max_ilda_rate = 0;
        uint32_t bulk_packet_sample_count = 0;
        uint32_t dac_min_val = 0;
        uint32_t dac_max_val = 0;
        
        uint32_t ilda_rate = 30000;

        std::string serial_number;

#ifdef LASERDOCKLIB_USB_SUPPORT
        LaserdockDevice *device = NULL;
#endif
    };
    
    ldUSBHardware(LaserdockDevice *device, QObject *parent = 0);
    ~ldUSBHardware();

    virtual QString id() const override;
    virtual QString hwType() const override;
    virtual QString address() const override;

    virtual int getDacRate() const override;
    virtual int getMaximumDacRate() const override;
    virtual void setDacRate(int rate) const override;
    virtual void setDacBufferTHold(int level) const override;

    bool send_security_requst(QByteArray request);
    bool get_security_response(QByteArray &response);
    
    struct device_params & params();
    
    virtual bool send_samples(uint startIndex, uint count) override;
    virtual int get_full_count() override;
        
private:
    QScopedPointer<ldUSBHardwarePrivate> d_ptr;
    Q_DECLARE_PRIVATE(ldUSBHardware)
};

#endif // LDUSBHARDWARE_H
