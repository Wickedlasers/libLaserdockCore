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
#include "ldCore/Hardware/ldHardwareInfo.h"
#include <QtDebug>

#ifdef LASERDOCKLIB_USB_SUPPORT
#include <laserdocklib/LaserdockDevice.h>
#endif

namespace cmds {
    const uint8_t LASERCUBE_SECURITY_CMD_REQUEST =                  0xb0;
    const uint8_t LASERCUBE_SECURITY_CMD_RESPONSE =                 0xb1;
}

class ldUSBHardwarePrivate {
public:
    
    ldUSBHardware *q_ptr;
    Q_DECLARE_PUBLIC(ldUSBHardware)
    
    ldUSBHardware::device_params params;

    
    ldUSBHardwarePrivate(ldUSBHardware *q) : q_ptr(q){

    }

    void initialize(){
#ifdef LASERDOCKLIB_USB_SUPPORT
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
        

//        bool enableOutput = true;
//        rc = enableOutput ? params.device->enable_output() : params.device->disable_output();
        rc = params.device->enable_output();

        if (!rc)
        {
            qWarning() << "Setting output failed";
            return;
        }       

        qDebug() << "Setting output output worked";
//        if (enableOutput) {
//            qDebug() << "Setting output output worked:" << enableOutput;
//        }
        
        q->setStatus(ldHardware::Status::INITIALIZED);
#endif
    }


    bool send(ldCompressedSample *samples, unsigned int count){

//        Q_Q(ldUsbHardware);

#ifdef LASERDOCKLIB_USB_SUPPORT
        // dirty conversion from ldCompressedSample to LaserdockSample...
        return params.device->send_samples((LaserdockSample * ) samples, count);
#else
        Q_UNUSED(samples)
        Q_UNUSED(count)
        return false;
#endif
    }

    bool usb_send(QByteArray ba) {
#ifdef LASERDOCKLIB_USB_SUPPORT
        return params.device->usb_send((unsigned char*) ba.data(), ba.length());
#else
        Q_UNUSED(ba)
        return false;
#endif
    }


    bool usb_get(QByteArray &ba) {
#ifdef LASERDOCKLIB_USB_SUPPORT
        unsigned char *output = params.device->usb_get((unsigned char*) ba.data(), ba.length());
        if(output) {
            ba = QByteArray::fromRawData((char *)output, 64);
        }
        return output != NULL;
#else
        Q_UNUSED(ba)
        return false;
#endif
    }


};

ldUSBHardware::ldUSBHardware(LaserdockDevice *device, QObject *parent)
    : ldHardware(parent)
    ,  d_ptr(new ldUSBHardwarePrivate(this))
{    
    Q_D(ldUSBHardware);
#ifdef LASERDOCKLIB_USB_SUPPORT
    d->params.device = device;
#else
    Q_UNUSED(device)
#endif
    d->initialize();

    // update hardware info with what we can get from the USB devices
    m_info->update_dacRate(d->params.ilda_rate);
    m_info->update_maxDacRate(m_info->get_dacRate());
    m_info->update_modelNumber(0);
    m_info->update_connectionType(0);
    m_info->update_modelName("USB LaserCube");
    m_info->update_fwMajor(d->params.version_major_number);
    m_info->update_fwMinor(d->params.version_minor_number);
    uint32_t bufsize;
    if (d->params.device->ringbuffer_empty_sample_count(&bufsize)) {
        m_info->update_bufferSize(bufsize);
    }

    m_info->update_hasValidInfo(true);
}

ldUSBHardware::~ldUSBHardware(){
#ifdef LASERDOCKLIB_USB_SUPPORT
    Q_D(ldUSBHardware);
    if(d->params.device) {
        delete d->params.device;
    }
#endif
}

QString ldUSBHardware::id() const
{
    Q_D(const ldUSBHardware);

    // TODO serial_number is not unique, use Jake's proposal instead https://github.com/Wickedlasers/laserdock_apps_cmake/issues/523
    return QString::fromStdString(d->params.device->get_serial_number());
}

QString ldUSBHardware::hwType() const
{
    return "USB";
}

QString ldUSBHardware::address() const
{
    //Q_D(const ldUSBHardware);
    //return QString::fromStdString(d->params.device->get_device_path());
    return QString();
}

ldUSBHardware::device_params &ldUSBHardware::params(){
    Q_D(ldUSBHardware);
    return d->params;
}


bool ldUSBHardware::send_samples(uint startIndex, uint count){
    Q_D(ldUSBHardware);

    // send nothing, it's ok
    if(count == 0)
        return true;

    Q_ASSERT(startIndex + count < m_compressed_buffer.size());
    bool ok = d->send(&m_compressed_buffer[startIndex], count);
    if(!ok) {
#ifdef LASERDOCKLIB_USB_SUPPORT
        qWarning() << __FUNCTION__ << "Can't send sample" << d->params.device->lastError();
#else
        qWarning() << __FUNCTION__ << "Can't send sample";
#endif
        setStatus(Status::UNKNOWN);
    }
    return ok;
}


bool ldUSBHardware::send_security_requst(QByteArray request){
    Q_D(ldUSBHardware);
    request.prepend(static_cast<int8_t>(cmds::LASERCUBE_SECURITY_CMD_REQUEST));
    return d->usb_send(request);
}

bool ldUSBHardware::get_security_response(QByteArray &response){
    Q_D(ldUSBHardware);
    response.append(static_cast<int8_t>(cmds::LASERCUBE_SECURITY_CMD_RESPONSE));
    bool ok = d->usb_get(response);
    if(ok)
        response.remove(0,3); // remove 0xb1 command and success bytes
    return ok;
}

int ldUSBHardware::get_full_count() {
#ifdef LASERDOCKLIB_USB_SUPPORT
    Q_D(ldUSBHardware);

    uint32_t remoteEmptyBuffer;
    bool isOk = d->params.device->ringbuffer_empty_sample_count(&remoteEmptyBuffer);
//    qDebug() << isOk << remoteEmptyBuffer;
    if (!isOk) {
        qWarning() << __FUNCTION__ << "Can't get ringbuffer_empty_sample_count";
        return -1;
    }

    Q_ASSERT(REMOTE_MAX_BUFFER >= remoteEmptyBuffer);

    return static_cast<int> (REMOTE_MAX_BUFFER - remoteEmptyBuffer);
#else
    return -1;
#endif
}

int ldUSBHardware::getDacRate() const
{
   Q_D(const ldUSBHardware);
   uint32_t tmp = 0;
   d->params.device->dac_rate(&tmp);
   return tmp;
}

int ldUSBHardware::getMaximumDacRate() const
{
    Q_D(const ldUSBHardware);
    return static_cast<int>(d->params.ilda_rate);
}

void ldUSBHardware::setDacRate(int rate) const
{
    if (rate>0) {
        Q_D(const ldUSBHardware);
        d->params.device->set_dac_rate(rate);
    }
}

void ldUSBHardware::setDacBufferTHold(int level) const
{

}
