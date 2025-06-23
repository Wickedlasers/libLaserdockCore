#include "ldCore/Hardware/ldNetworkHardware.h"
#include "ldCore/Hardware/ldHardwareInfo.h"

#include <QtDebug>
#include <ldCore/ldCore.h>

#include <laserdocklib/LaserDockNetworkDevice.h>

class ldNetworkHardwarePrivate {
public:

    ldNetworkHardware *q_ptr;
    Q_DECLARE_PUBLIC(ldNetworkHardware)

    ldNetworkHardware::device_params params;


    ldNetworkHardwarePrivate(ldNetworkHardware *q) : q_ptr(q){

    }

    void initialize(){
        Q_Q(ldNetworkHardware);
        bool rc;
        rc = params.device->disable_output();
        if (!rc)
        {
            qWarning() << "Disable output failed";
            return;
        }
        qDebug() << "Disable output worked";

        qDebug() << "===Running===";

        rc = params.device->enable_buffer_state_replies();


        rc = params.device->set_dac_rate(params.ilda_rate);
        if (!rc)
        {
            qWarning() << "setting ILDA rate failed";
            return;
        }

        qDebug() << "Setting ILDA rate worked:" << params.ilda_rate << "pps";

        /*

         // this is now done in overidden setActive function
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
        */

        q->setStatus(ldHardware::Status::INITIALIZED);
        //q->setStatus(ldHardware::Status::UNKNOWN);
    }

    bool send(LaserdockSample *samples, unsigned int count){

//        Q_Q(ldUsbHardware);

        // dirty conversion from ldCompressedSample to LaserdockSample...
        return params.device->send_samples(samples, count);
    }
};


ldNetworkHardware::ldNetworkHardware(LaserdockNetworkDevice *device, QObject *parent)  : ldHardware(parent)
  ,  d_ptr(new ldNetworkHardwarePrivate(this))
{
    Q_D(ldNetworkHardware);
    d->params.device = device;

    // info class must always exist in main gui thread
    m_info->moveToThread(ldCore::instance()->thread());

    connect(device,&LaserdockNetworkDevice::BatteryPercentUpdated,m_info,&ldHardwareInfo::update_batteryPercent);
    connect(device,&LaserdockNetworkDevice::ModelNumberUpdated,m_info,&ldHardwareInfo::update_modelNumber);
    connect(device,&LaserdockNetworkDevice::DACRateUpdated,m_info,&ldHardwareInfo::update_dacRate);
    connect(device,&LaserdockNetworkDevice::MaxDACRateUpdated,m_info,&ldHardwareInfo::update_maxDacRate);
    connect(device,&LaserdockNetworkDevice::TemperatureUpdated,m_info,&ldHardwareInfo::update_temperatureDegC);
    connect(device,&LaserdockNetworkDevice::ModelNameUpdated,m_info,&ldHardwareInfo::update_modelName);
    connect(device,&LaserdockNetworkDevice::SampleBufferSizeUpdated,m_info,&ldHardwareInfo::update_bufferSize);
    connect(device,&LaserdockNetworkDevice::SampleBufferFreeUpdated,m_info,&ldHardwareInfo::update_bufferFree);
    connect(device,&LaserdockNetworkDevice::FWMajorRevisionUpdated,m_info,&ldHardwareInfo::update_fwMajor);
    connect(device,&LaserdockNetworkDevice::FWMinorRevisionUpdated,m_info,&ldHardwareInfo::update_fwMinor);
    connect(device,&LaserdockNetworkDevice::PacketErrorsUpdated,m_info,&ldHardwareInfo::update_packetErrors);
    connect(device,&LaserdockNetworkDevice::IPAddressUpdated,m_info,&ldHardwareInfo::update_address);
    connect(device,&LaserdockNetworkDevice::OverTemperatureUpdated,m_info,[&](bool en){
        m_info->update_overTemperature((en) ? 1 : 0);
    });
    connect(device,&LaserdockNetworkDevice::TemperatureWarningUpdated,m_info,[&](bool en){
        m_info->update_temperatureWarning((en) ? 1 : 0);
    });

    connect(device,&LaserdockNetworkDevice::InterlockEnabledUpdated,m_info,[&](bool en){
        m_info->update_interlockEnabled((en) ? 1 : 0);
    });

    connect(device,&LaserdockNetworkDevice::ConnectionTypeUpdated,[&](LaserdockNetworkDevice::ConnectionType ct){
        m_info->update_connectionType(static_cast<int>(ct));
    });

    // get initial state of device
    m_info->update_interlockEnabled(device->get_interlock_enabled() ? 1 : 0);
    m_info->update_temperatureWarning(device->get_temperature_warning() ? 1 : 0);
    m_info->update_overTemperature(device->get_over_temperature() ? 1 : 0);
    m_info->update_address(device->get_ip_address());

    m_info->update_hasValidInfo(true); // network device always has valid info available

}

void ldNetworkHardware::setActive(bool active)
{
    Q_D(ldNetworkHardware);
    bool rc = false;
    rc = active ? d->params.device->enable_output() : d->params.device->disable_output();
    if (!rc)
    {
        qWarning() << "Setting output failed";
        return;
    }


     qDebug() << "Output enable was set to:" << active;
     m_isActive = active;
}

void ldNetworkHardware::ResetStatus()
{
    setStatus(Status::UNKNOWN);
    params().device->ResetStatus();
    m_isActive = false;
}

void ldNetworkHardware::initialize()
{
    Q_D(ldNetworkHardware);
    d->initialize();
}

ldNetworkHardware::~ldNetworkHardware()
{
    Q_D(ldNetworkHardware);
    //setActive(false);
    if(d->params.device) {
        delete d->params.device;
    }
}

QString ldNetworkHardware::id() const
{
    return QString::fromStdString(params().device->get_serial_number());
}

QString ldNetworkHardware::hwType() const
{
    return "Network";
}

QString ldNetworkHardware::address() const
{
    return params().device->get_ip_address();
}

bool ldNetworkHardware::send_samples(uint startIndex, uint count)
{
    Q_ASSERT((startIndex + count < m_compressed_buffer.size())
             || (count == 0));

    if(count > 0)
        return send_samples((LaserdockSample*) &m_compressed_buffer[startIndex], count);
    else
        return send_samples(nullptr, 0);
}

bool ldNetworkHardware::send_samples(LaserdockSample *samples, unsigned int size){
    Q_D(ldNetworkHardware);
    bool ok = d->send(samples, size);

    if(ok==false) {
        setStatus(Status::UNKNOWN);
        emit deviceDisconnected();
    }
    return ok;
}


int ldNetworkHardware::get_full_count() {
    Q_D(ldNetworkHardware);

    uint32_t sample_space_left = 0;
    uint32_t remote_buffer_size_samples = 0;

    // get the sample space left in the buffer
    bool ret1 = d->params.device->ringbuffer_empty_sample_count(&sample_space_left);
    if (!ret1) return -1;
    ret1=d->params.device->ringbuffer_sample_count(&remote_buffer_size_samples);
    if (!ret1) return -1;

    //qDebug() <<  free;
    //if (!ret1) return -1;

    //return count1;

    int rbuffer = remote_buffer_size_samples - sample_space_left; // calc num samples currently in buffer
    rbuffer = std::max(rbuffer, -1);
    rbuffer = std::min(rbuffer, (int)remote_buffer_size_samples);

    //qDebug() << "samples currently in remote sample buffer:" << rbuffer;
    return rbuffer;
}

const ldNetworkHardware::device_params &ldNetworkHardware::params() const{
    Q_D(const ldNetworkHardware);
    return d->params;
}

int ldNetworkHardware::getDacRate() const
{
   Q_D(const ldNetworkHardware);
   uint32_t tmp = 0;
   d->params.device->dac_rate(&tmp);
   return static_cast<int>(tmp);
}

int ldNetworkHardware::getMaximumDacRate() const
{
    Q_D(const ldNetworkHardware);
    uint32_t tmp = 0;
    d->params.device->max_dac_rate(&tmp);
    return static_cast<int>(tmp);
}

void ldNetworkHardware::setDacRate(int rate) const
{
    if (rate>0) {
        Q_D(const ldNetworkHardware);
        d->params.device->set_dac_rate(rate);
    }
}

void ldNetworkHardware::setDacBufferTHold(int level) const
{
    if (level > 0) {
        Q_D(const ldNetworkHardware);
        d->params.device->set_dac_buffer_thold_lvl(level);
    }
}
