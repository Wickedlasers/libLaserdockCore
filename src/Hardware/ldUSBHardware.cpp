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

#include "ldCore/Hardware/ldUSBHardware.h"

#include <QtDebug>

#include <laserdocklib/LaserdockDevice.h>

class ldUSBHardwarePrivate {
public:
    
    ldUSBHardware *q_ptr;
    Q_DECLARE_PUBLIC(ldUSBHardware)
    
    ldUSBHardware::device_params params;

    
    ldUSBHardwarePrivate(ldUSBHardware *q) : q_ptr(q){

    }

    void initialize(){
        Q_Q(ldUSBHardware);

        bool rc; uint32_t temp;
        
        if (!params.device) {
            qDebug() << "Error finding USB device";
            return;
        }

        std::string serialNumber = params.device->get_serial_number();
        if (serialNumber.empty())
        {
            qWarning() << "Getting serial number failed.";
            return;
        } else {
            params.serial_number = serialNumber;
            qDebug() << "Getting serial number:" << QString::fromStdString(params.serial_number);
        }


        rc = params.device->version_major_number(&params.version_major_number);
        if (!rc)
        {
            qWarning() << "Getting FW Major version failed.";
            return;
        }
        qDebug() << "Getting FW Major version:" << params.version_major_number;

        rc = params.device->version_minor_number(&params.version_minor_number);
        if (!rc)
        {
            qWarning() << "Getting FW Minor version failed";
            return;
        }
        qDebug() << "Getting FW Minor version:" << params.version_minor_number;

        qDebug() << "Clearing ringbuffer";
        rc = params.device->clear_ringbuffer();
        if (!rc) {
            qWarning() << "Clearing ringbuffer buffer failed";
            return;
        }
        
        
        rc = params.device->bulk_packet_sample_count(&params.bulk_packet_sample_count);
        if (!rc)
        {
            qWarning() << "Getting bulk packet sample count failed";
            return;
        }
        qDebug() << "Getting bulk packet sample count:" << params.bulk_packet_sample_count;
        

        rc = params.device->max_dac_rate(&params.max_ilda_rate);
        if (!rc)
        {
            qWarning() << "Getting max ilda rate failed";
            return;
        }
        qDebug() << "Getting max ilda rate:" << params.max_ilda_rate << "pps";


        rc = params.device->min_dac_value(&params.dac_min_val);
        if (!rc)
        {
            qWarning() << ("Getting dac min failed");
            return;
        }
        qDebug() << "Getting dac min:" << params.dac_min_val;
        
        
        rc = params.device->max_dac_value(&params.dac_max_val);
        if (!rc) {
            qWarning() << "Getting dac max failed";
            return;
        }
        qDebug() << "getting dac max:" << params.dac_max_val;
        
        
        rc = params.device->ringbuffer_empty_sample_count(&temp);
        if (!rc)
        {
            qWarning() << "Getting ringbuffer empty sample count failed";
            return;
        }
        qDebug() << "Getting ringbuffer empty sample count:" << temp;
        

        rc = params.device->disable_output();
        if (!rc)
        {
            qWarning() << "Disable output failed";
            return;
        }
        qDebug() << "Disable output worked";
        
        qDebug() << "===Running===";
        

        rc = params.device->set_dac_rate(params.ilda_rate);
        if (!rc)
        {
            qWarning() << "setting ILDA rate failed";
            return;
        }
        
        qDebug() << "Setting ILDA rate worked:" << params.ilda_rate << "pps";
        

        uint32_t enable = 1;
        rc = enable ? params.device->enable_output() : params.device->disable_output();
        if (!rc)
        {
            qWarning() << "Setting output failed";
            return;
        }
        
        if (enable) {
            qDebug() << "Setting output output worked:" << enable;
        }
        
        q->setStatus(ldHardware::Status::INITIALIZED);
    }


    bool send(LaserdockSample *samples, unsigned int count){

//        Q_Q(ldUsbHardware);

        return params.device->send_samples(samples, count);
    }

    bool usb_send(QByteArray ba) {
        return params.device->usb_send((unsigned char*) ba.data(), ba.length());
    }


    bool usb_get(QByteArray &ba) {
        unsigned char *output = params.device->usb_get((unsigned char*) ba.data(), ba.length());
        if(output) {
            ba = QByteArray::fromRawData((char *)output, 64);
        }
        return output != NULL;
    }


};

ldUSBHardware::ldUSBHardware(LaserdockDevice *device, QObject *parent)
    : ldHardware(parent)
    ,  d_ptr(new ldUSBHardwarePrivate(this))
{    
    Q_D(ldUSBHardware);
    d->params.device = device;
    d->initialize();
}

ldUSBHardware::~ldUSBHardware(){
    Q_D(ldUSBHardware);
    if(d->params.device) {
        delete d->params.device;
    }
}


ldUSBHardware::device_params &ldUSBHardware::params(){
    Q_D(ldUSBHardware);
    return d->params;
}


bool ldUSBHardware::send_samples(LaserdockSample *samples, unsigned int size){
    Q_D(ldUSBHardware);
    bool ok = d->send(samples, size);
    if(!ok) {
        setStatus(Status::UNKNOWN);
        emit deviceDisconnected();
    }
    return ok;
}


void ldUSBHardware::send_security_requst(QByteArray request){
    Q_D(ldUSBHardware);
    d->usb_send(request);
    
}

void ldUSBHardware::get_security_response(QByteArray &response){
    Q_D(ldUSBHardware);
    d->usb_get(response);
}

int ldUSBHardware::get_full_count() {
    Q_D(ldUSBHardware);

    uint32_t count1 = REMOTE_MAX_BUFFER;
	bool ret1 = d->params.device->ringbuffer_empty_sample_count(&count1);

//    qDebug() << ret1 << count1;
	if (!ret1) return -1;

    int rbuffer = REMOTE_MAX_BUFFER - count1;
    rbuffer = std::max(rbuffer, -1);
    rbuffer = std::min(rbuffer, REMOTE_MAX_BUFFER);
    return rbuffer;
}
