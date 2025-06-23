#ifndef LDDEVICEBUFFERCONFIGDELEGATOR_H
#define LDDEVICEBUFFERCONFIGDELEGATOR_H

#include <QtCore/QObject>

#include <laserdocklib/LaserDockNetworkDevice.h>

#include <ldCore/ldCore_global.h>

class ldHardware;

class LDCORESHARED_EXPORT ldDeviceBufferConfigDelegator : public QObject
{
    Q_OBJECT

public:
    struct DeviceBufferConfig{
        uint samples_per_packet; // how many samples to send
        int remote_buffer_cutoff; // this sets the buffer point at which we stop sending to remote device
        int wait_connect_sleep_ms;
        int wait_buffer_sleep_ms;
        int sleep_after_packet_send_ms; // how long to sleep thread for after sending data to a device (in ms)
        uint max_samples_per_udp_xfer; // max number of samples to transmit over any single UDP transer (set to 0 for USB devices)
        uint max_udp_packets_per_xfer;
    } ;

    explicit ldDeviceBufferConfigDelegator(QObject *parent = nullptr);
    ~ldDeviceBufferConfigDelegator() override;

    DeviceBufferConfig getBufferConfig()  const;

    void updateBufferingStrategy(std::vector<ldHardware*> devices);

private:
    void updateNetworkBufferingStrategy(QList<LaserdockNetworkDevice::ConnectionType> &contypes);

    const DeviceBufferConfig m_usb_device_config{768,1024,12,6,0,0,0}; // original values for USB cube

#if defined(Q_OS_ANDROID)
      const DeviceBufferConfig m_wifi_server_device_config{300,2400,12,4,2,140,10}; // buffering strategy for a wifi server network cube
      const DeviceBufferConfig m_wifi_server_client_device_config{600,2500,12,5,10,140,10}; // buffering strategy for a wifi server/client(s) network cube
      const DeviceBufferConfig m_wifi_client_device_config{300,3000,12,2,2,100,20}; // buffering strategy for a wifi client network cube
      const DeviceBufferConfig m_wifi_dual_client_device_config{300,4000,12,2,2,100,10}; // buffering strategy for a wifi client network cube
      const DeviceBufferConfig m_lan_server_device_config{300,1800,12,4,2,140,10}; // buffering strategy for ethernet server network cube
      const DeviceBufferConfig m_lan_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
      const DeviceBufferConfig m_lan_dual_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
#elif defined(Q_OS_IOS)
    const DeviceBufferConfig m_wifi_server_device_config{300,3000,12,8,8,140,20}; // buffering strategy for a wifi server network cube
    const DeviceBufferConfig m_wifi_server_client_device_config{700,2800,12,10,10,140,10}; // buffering strategy for a wifi server network cube
    const DeviceBufferConfig m_wifi_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for a wifi client network cube
    const DeviceBufferConfig m_wifi_dual_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for a wifi client network cube
    const DeviceBufferConfig m_lan_server_device_config{300,1800,12,4,2,140,10}; // buffering strategy for ethernet server network cube
    const DeviceBufferConfig m_lan_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
    const DeviceBufferConfig m_lan_dual_client_device_config{300,2400,12,4,2,140,10}; // buffering strategy for ethernet client network cube
#else

    const DeviceBufferConfig m_wifi_server_device_config{600,2000,12,4,10,140,20}; // buffering strategy for a wifi server network cube
    const DeviceBufferConfig m_wifi_server_client_device_config{600,2500,12,5,10,140,10}; // buffering strategy for a wifi server/client(s) network cube
    const DeviceBufferConfig m_wifi_client_device_config{700,1800,12,6,4,80,20}; // buffering strategy for a wifi client network cube
    const DeviceBufferConfig m_wifi_dual_client_device_config{700,1500,12,6,8,140,10}; // buffering strategy for 2x wifi client network cube
    const DeviceBufferConfig m_lan_server_device_config{700,1800,12,6,4,80,20}; // buffering strategy for ethernet server network cube
    const DeviceBufferConfig m_lan_client_device_config{700,1800,12,6,4,80,20}; // buffering strategy for ethernet client network cube
    const DeviceBufferConfig m_lan_dual_client_device_config{700,1800,12,6,4,140,10}; // buffering strategy for ethernet client network cube
#endif

    const DeviceBufferConfig* m_currentBufferConfig{nullptr}; // points to the currently selected device buffering configuration
};

#endif // LDDEVICEBUFFERCONFIGDELEGATOR_H
